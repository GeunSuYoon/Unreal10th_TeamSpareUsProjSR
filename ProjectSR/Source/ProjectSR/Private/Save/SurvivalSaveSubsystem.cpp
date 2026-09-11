#include "Save/SurvivalSaveSubsystem.h"

#include "Save/SurvivalSaveGame.h"
#include "Framework/SurvivalLoopActor.h"
#include "Player/PlayerCharacter.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "Component/CraftingComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/StatComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogSurvivalSave, Log, All);

const FString USurvivalSaveSubsystem::SlotName(TEXT("SurvivalAutoSave"));

bool USurvivalSaveSubsystem::HasSaveGame() const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

void USurvivalSaveSubsystem::CaptureInventory(const UInventoryComponent* Inventory, int32& OutSize,
	TArray<FInventorySlotSaveData>& OutSlots)
{
	OutSize = 0;
	OutSlots.Reset();
	if (!IsValid(Inventory)) return;

	OutSize = Inventory->GetSize();
	const TArray<FInventorySlot> Slots = Inventory->GetCopiedSlots();
	OutSlots.Reserve(OutSize);
	for (int32 Index = 0; Index < OutSize; ++Index)
	{
		FInventorySlotSaveData& SavedSlot = OutSlots.AddDefaulted_GetRef();
		if (Slots.IsValidIndex(Index) && !Slots[Index].IsEmpty())
		{
			SavedSlot.ItemData = const_cast<UItemDataAsset*>(Slots[Index].ItemData.Get());
			SavedSlot.Count = Slots[Index].GetCount();
		}
	}
}

bool USurvivalSaveSubsystem::ResolveInventory(int32 SavedSize,
	const TArray<FInventorySlotSaveData>& SavedSlots, TArray<FInventorySlot>& OutSlots)
{
	if (SavedSize < 0 || SavedSlots.Num() != SavedSize) return false;

	OutSlots.Reset();
	OutSlots.SetNum(SavedSize);
	for (int32 Index = 0; Index < SavedSize; ++Index)
	{
		const FInventorySlotSaveData& SavedSlot = SavedSlots[Index];
		if (SavedSlot.Count == 0 && SavedSlot.ItemData.IsNull()) continue;

		UItemDataAsset* ItemData = SavedSlot.ItemData.LoadSynchronous();
		if (!IsValid(ItemData) || SavedSlot.Count <= 0 || SavedSlot.Count > ItemData->MaxStackCount)
		{
			UE_LOG(LogSurvivalSave, Error, TEXT("Invalid saved inventory slot %d."), Index);
			return false;
		}
		OutSlots[Index].ItemData = ItemData;
		OutSlots[Index].SetCount(SavedSlot.Count);
	}

	return true;
}

bool USurvivalSaveSubsystem::SaveCompletedDay(const ASurvivalLoopActor* SurvivalLoop) const
{
	if (!IsValid(SurvivalLoop) || SurvivalLoop->CurrentDay < 1
		|| !IsValid(SurvivalLoop->Player) || !IsValid(SurvivalLoop->SpaceShip))
	{
		return false;
	}

	const UStatComponent* PlayerStats = SurvivalLoop->Player->FindComponentByClass<UStatComponent>();
	const UInventoryComponent* PlayerInventory = SurvivalLoop->Player->FindComponentByClass<UInventoryComponent>();
	const UInventoryComponent* Warehouse = SurvivalLoop->SpaceShip->GetWarehouse();
	const ULazerComponent* Lazer = SurvivalLoop->SpaceShip->GetLazerComponent();
	if (!IsValid(PlayerStats) || !IsValid(PlayerInventory) || !IsValid(Warehouse) || !IsValid(Lazer)) return false;

	USurvivalSaveGame* Save = Cast<USurvivalSaveGame>(
		UGameplayStatics::CreateSaveGameObject(USurvivalSaveGame::StaticClass()));
	if (!IsValid(Save)) return false;

	Save->CompletedDay = SurvivalLoop->CurrentDay;
	Save->PlayerHealth = PlayerStats->GetHealth();
	Save->PlayerOxygen = PlayerStats->GetOxygen();
	Save->SpaceShipStat = SurvivalLoop->SpaceShip->GetStat();
	Save->LazerStat = Lazer->GetStat();
	Save->SpaceShipDurability = SurvivalLoop->SpaceShip->GetCurrentDurability();
	Save->SpaceShipEnergy = SurvivalLoop->SpaceShip->GetCurrentEnergy();
	CaptureInventory(PlayerInventory, Save->PlayerInventorySize, Save->PlayerInventory);
	CaptureInventory(Warehouse, Save->WarehouseSize, Save->Warehouse);

	const bool bSaved = UGameplayStatics::SaveGameToSlot(Save, SlotName, UserIndex);
	if (bSaved)
	{
		UE_LOG(LogSurvivalSave, Log, TEXT("Day %d checkpoint save succeeded."), Save->CompletedDay);
	}
	else
	{
		UE_LOG(LogSurvivalSave, Error, TEXT("Day %d checkpoint save failed."), Save->CompletedDay);
	}
	return bSaved;
}

bool USurvivalSaveSubsystem::TryApplyRequestedLoad(ASurvivalLoopActor* SurvivalLoop)
{
	if (!bLoadRequested) return true;
	if (!IsValid(SurvivalLoop) || !IsValid(SurvivalLoop->Player) || !IsValid(SurvivalLoop->SpaceShip)) return false;

	USurvivalSaveGame* Save = Cast<USurvivalSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!IsValid(Save) || Save->SaveVersion != USurvivalSaveGame::CurrentSaveVersion || Save->CompletedDay < 1)
	{
		UE_LOG(LogSurvivalSave, Error, TEXT("The survival save is missing, corrupt, or uses an unsupported version."));
		return false;
	}

	UStatComponent* PlayerStats = SurvivalLoop->Player->FindComponentByClass<UStatComponent>();
	UInventoryComponent* PlayerInventory = SurvivalLoop->Player->FindComponentByClass<UInventoryComponent>();
	ASpaceShipActor* Ship = SurvivalLoop->SpaceShip;
	if (!IsValid(PlayerStats) || !IsValid(PlayerInventory) || !IsValid(Ship->GetWarehouse())
		|| !IsValid(Ship->GetLazerComponent()))
	{
		return false;
	}
	if (!FMath::IsFinite(Save->PlayerHealth) || Save->PlayerHealth <= 0.0f
		|| !FMath::IsFinite(Save->PlayerOxygen) || Save->PlayerOxygen < 0.0f
		|| Save->SpaceShipStat.Level < 0 || !FMath::IsFinite(Save->SpaceShipStat.MaxDurability)
		|| Save->SpaceShipStat.MaxDurability <= 0.0f || !FMath::IsFinite(Save->SpaceShipStat.MaxEnergy)
		|| Save->SpaceShipStat.MaxEnergy < 0.0f || !FMath::IsFinite(Save->SpaceShipDurability)
		|| Save->SpaceShipDurability <= 0.0f || !FMath::IsFinite(Save->SpaceShipEnergy)
		|| Save->SpaceShipEnergy < 0.0f || Save->LazerStat.Level < 0)
	{
		UE_LOG(LogSurvivalSave, Error, TEXT("The survival save contains invalid stat values."));
		return false;
	}

	// Validate and restore inventories before committing scalar state. A corrupt item reference
	// must not leave a partially restored run that will immediately overwrite the checkpoint.
	TArray<FInventorySlot> ResolvedPlayerInventory;
	TArray<FInventorySlot> ResolvedWarehouse;
	if (!ResolveInventory(Save->PlayerInventorySize, Save->PlayerInventory, ResolvedPlayerInventory)
		|| !ResolveInventory(Save->WarehouseSize, Save->Warehouse, ResolvedWarehouse))
	{
		return false;
	}
	if (!PlayerInventory->RestoreSlots(Save->PlayerInventorySize, ResolvedPlayerInventory)
		|| !Ship->GetWarehouse()->RestoreSlots(Save->WarehouseSize, ResolvedWarehouse)) return false;

	Ship->RestoreRuntimeState(Save->SpaceShipStat, Save->LazerStat,
		Save->SpaceShipDurability, Save->SpaceShipEnergy);
	PlayerStats->RestoreCurrentStats(Save->PlayerHealth, Save->PlayerOxygen);
	if (USpaceShipUpgradeComponent* Upgrade = Ship->GetUpgradeComponent())
	{
		Upgrade->RestoreRecipeUnlocks();
	}
	SurvivalLoop->CurrentDay = Save->CompletedDay;
	SurvivalLoop->Player->InitBroadCast();
	Ship->InitBroadCast();
	bLoadRequested = false;

	UE_LOG(LogSurvivalSave, Log, TEXT("Loaded checkpoint after day %d."), Save->CompletedDay);
	return true;
}

bool USurvivalSaveSubsystem::DeleteSaveGame()
{
	bLoadRequested = false;
	return !HasSaveGame() || UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}
