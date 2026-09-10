#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/MachineArmComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"

namespace
{
    // The outer Level is the target level; row names are free to change.
    template<typename T>
    const T* FindNextRow(const UDataTable* Table, int32 CurrentLevel, EUpgradeResult& Result)
    {
        Result = EUpgradeResult::InvalidData;
		if (!Table || Table->GetRowStruct() != T::StaticStruct() || CurrentLevel < 0 || CurrentLevel == MAX_int32)
		{
            return nullptr;
		}
        TArray<T*>	Rows;
        TSet<int32>	Levels;
        int32		MaxLevel = -1;
        const T*	Next = nullptr;
		
		Table->GetAllRows<T>(TEXT("Ship upgrade"), Rows);
        for (const T* Row : Rows)
        {
            if (!Row || Row->Level < 0 || Levels.Contains(Row->Level)) return nullptr;
            Levels.Add(Row->Level);
            MaxLevel = FMath::Max(MaxLevel, Row->Level);
            if (Row->Level == CurrentLevel + 1) Next = Row;
        }
        if (Next) Result = EUpgradeResult::Success;
        else if (MaxLevel >= 0 && CurrentLevel == MaxLevel) Result = EUpgradeResult::MaxLevel;
		return (Next);
    }

    bool NormalizeIngredients(TArray<FIngredient>& Ingredients)
    {
        TArray<FIngredient>	Normalized;

        for (const FIngredient&	Ingredient : Ingredients)
        {
			if (!IsValid(Ingredient.ItemData.Get()) || Ingredient.Quantity <= 0) return (false);
            FIngredient* Existing = Normalized.FindByPredicate([&](const FIngredient& Other)
            {
				return Other.ItemData == Ingredient.ItemData;
				});
            if (Existing)
            {
                const int64 Total = static_cast<int64>(Existing->Quantity) + Ingredient.Quantity;
                if (Total > MAX_int32) return false;
                Existing->Quantity = static_cast<int32>(Total);
            }
            else Normalized.Add(Ingredient);
        }
        Ingredients = MoveTemp(Normalized);
		return (true);
    }
	bool NonNegative(float Value) { return (FMath::IsFinite(Value) && Value >= 0.f); }
}

USpaceShipUpgradeComponent::USpaceShipUpgradeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USpaceShipUpgradeComponent::BeginPlay()
{
    Super::BeginPlay();
    if (!CraftingComponent && GetOwner()) CraftingComponent = GetOwner()->FindComponentByClass<UCraftingComponent>();
    RestoreRecipeUnlocks();
}

bool USpaceShipUpgradeComponent::InitializeLevelZero()
{
    if (bUpgradeInProgress) return false;
    if (bLevelZeroInitialized) return true;

    ASpaceShipActor* Ship = Cast<ASpaceShipActor>(GetOwner());
    if (!IsValid(Ship) || Ship->GetStat().Level != 0
        || !SpaceShipUpgradeTable || SpaceShipUpgradeTable->GetRowStruct() != FSpaceShipUpgrade::StaticStruct())
        return false;

    TArray<FSpaceShipUpgrade*> Rows;
    SpaceShipUpgradeTable->GetAllRows<FSpaceShipUpgrade>(TEXT("Initialize ship level zero"), Rows);
    const FSpaceShipUpgrade* InitialRow = nullptr;
    for (const FSpaceShipUpgrade* Row : Rows)
    {
        if (Row && Row->Level == 0)
        {
            if (InitialRow) return false;
            InitialRow = Row;
        }
    }
    if (!InitialRow) return false;

    FSpaceShipStat InitialStat = InitialRow->SpaceShipStat;
    InitialStat.Level = 0;
    if (!NonNegative(InitialStat.MaxDurability) || !NonNegative(InitialStat.MaxEnergy)
        || !NonNegative(InitialStat.MaxCapacity) || !NonNegative(InitialStat.OperationalEnergy)
        || !NonNegative(InitialStat.MoveSpeed)) return false;

    if (!IsValid(CraftingComponent)) CraftingComponent = Ship->FindComponentByClass<UCraftingComponent>();
    const TArray<FName> Recipes = InitialRow->UnlockRecipeIds;
    for (FName Recipe : Recipes)
        if (!IsValid(CraftingComponent) || !CraftingComponent->HasRecipe(Recipe)) return false;

    // Validate everything before changing stats. Initial equipment does not consume ingredients.
    {
        TGuardValue<bool> Guard(bUpgradeInProgress, true);
        Ship->ApplySpaceShipStat(InitialStat);
        for (FName Recipe : Recipes) CraftingComponent->Unlock(Recipe);
        bLevelZeroInitialized = true;
    }
    OnUpgradeStateChanged.Broadcast();
    return true;
}

void USpaceShipUpgradeComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	for (UInventoryComponent* Inventory : Inventories)
	{
		if (IsValid(Inventory))
		{
			Inventory->OnSlotChanged.RemoveDynamic(this, &ThisClass::HandleInventoryChanged);
		}
	}
    Super::EndPlay(Reason);
}

TArray<UInventoryComponent*> USpaceShipUpgradeComponent::GetInventories() const
{
    TArray<UInventoryComponent*>	Result;

    for (UInventoryComponent* Inventory : Inventories) Result.Add(Inventory);
    return Result;
}

void USpaceShipUpgradeComponent::SetInventories(const TArray<UInventoryComponent*>& InInventories)
{
    if (bUpgradeInProgress) return;
	for (UInventoryComponent* Inventory : Inventories)
	{
		if (IsValid(Inventory))
		{
			Inventory->OnSlotChanged.RemoveDynamic(this, &ThisClass::HandleInventoryChanged);
		}
	}
    Inventories.Reset();
	for (UInventoryComponent* Inventory : InInventories)
	{
		if (IsValid(Inventory))
		{
			Inventories.AddUnique(Inventory);
		}
	}
	for (UInventoryComponent* Inventory : Inventories)
	{
        Inventory->OnSlotChanged.AddUniqueDynamic(this, &ThisClass::HandleInventoryChanged);
	}
    OnUpgradeStateChanged.Broadcast();
}

void USpaceShipUpgradeComponent::SetCraftingComponent(UCraftingComponent* InComponent)
{
    if (bUpgradeInProgress) return;
    CraftingComponent = InComponent;
    RestoreRecipeUnlocks();
    OnUpgradeStateChanged.Broadcast();
}

void USpaceShipUpgradeComponent::HandleInventoryChanged(int32 SlotIndex)
{
    if (!bUpgradeInProgress) OnUpgradeStateChanged.Broadcast();
}

int32 USpaceShipUpgradeComponent::GetOwnedIngredientCount(UItemDataAsset* ItemData) const
{
    int64 Count = 0;
    for (UInventoryComponent* Inventory : Inventories)
        if (IsValid(Inventory)) Count += Inventory->GetSpendableItemCount(ItemData);
    return static_cast<int32>(FMath::Min<int64>(Count, MAX_int32));
}

FShipUpgradePreview USpaceShipUpgradeComponent::GetPreview(EUpgradeTarget Target) const
{
    FShipUpgradePreview Preview;
    const ASpaceShipActor* Ship = Cast<ASpaceShipActor>(GetOwner());
    if (!IsValid(Ship)) return Preview;
    Preview.CurrentShip = Ship->GetStat();
    if (Ship->GetLazerComponent()) Preview.CurrentLazer = Ship->GetLazerComponent()->GetStat();
    if (Ship->GetMachineArmComponent()) Preview.CurrentArm = Ship->GetMachineArmComponent()->GetStat();
    switch (Target)
    {
    case EUpgradeTarget::SpaceShip:
        Preview.CurrentLevel = Preview.CurrentShip.Level;
        if (const auto* Row = FindNextRow<FSpaceShipUpgrade>(SpaceShipUpgradeTable, Preview.CurrentLevel, Preview.Result))
        {
            Preview.NextShip = Row->SpaceShipStat;
            Preview.NextShip.Level = Row->Level;
            Preview.Ingredients = Row->Ingredients;
            Preview.UnlockRecipeIds = Row->UnlockRecipeIds;
            Preview.bHasNextLevel = true;
            const auto& S = Preview.NextShip;
            if (!NonNegative(S.MaxDurability) || !NonNegative(S.MaxEnergy) || !NonNegative(S.MaxCapacity)
                || !NonNegative(S.OperationalEnergy) || !NonNegative(S.MoveSpeed))
                Preview.Result = EUpgradeResult::InvalidData;
        }
        break;
    case EUpgradeTarget::Lazer:
        if (!Ship->GetLazerComponent()) return Preview;
        Preview.CurrentLevel = Preview.CurrentLazer.Level;
        if (const auto* Row = FindNextRow<FLazerUpgrade>(LazerUpgradeTable, Preview.CurrentLevel, Preview.Result))
        {
            Preview.NextLazer = Row->LazerStat;
            Preview.NextLazer.Level = Row->Level;
            Preview.Ingredients = Row->Ingredients;
            Preview.bHasNextLevel = true;
            const auto& S = Preview.NextLazer;
            if (!NonNegative(S.Damage) || !NonNegative(S.ReactiveEnergy) || !NonNegative(S.OperationalEnergy))
                Preview.Result = EUpgradeResult::InvalidData;
        }
        break;
    case EUpgradeTarget::MachineArm:
        if (!Ship->GetMachineArmComponent()) return Preview;
        Preview.CurrentLevel = Preview.CurrentArm.Level;
        if (const auto* Row = FindNextRow<FMachineArmUpgrade>(MachineArmUpgradeTable, Preview.CurrentLevel, Preview.Result))
        {
            Preview.NextArm = Row->MachineArmStat;
            Preview.NextArm.Level = Row->Level;
            Preview.Ingredients = Row->Ingredients;
            Preview.bHasNextLevel = true;
            const auto& S = Preview.NextArm;
            if (!NonNegative(S.ItemCollectTime) || S.ItemCollectTime == 0.f
                || !NonNegative(S.ItemCollectWeight) || !NonNegative(S.OperationalEnergy))
                Preview.Result = EUpgradeResult::InvalidData;
        }
        break;
    default: return Preview;
    }
    Preview.NextLevel = Preview.CurrentLevel < MAX_int32 ? Preview.CurrentLevel + 1 : Preview.CurrentLevel;
    if (Preview.Result != EUpgradeResult::Success) return Preview;
    if (!NormalizeIngredients(Preview.Ingredients))
    {
        Preview.Result = EUpgradeResult::InvalidData;
        return Preview;
    }
    for (FName RecipeId : Preview.UnlockRecipeIds)
        if (!IsValid(CraftingComponent) || !CraftingComponent->HasRecipe(RecipeId))
        {
            Preview.Result = EUpgradeResult::InvalidData;
            return Preview;
        }
    if (Target == EUpgradeTarget::Lazer && Preview.NextLevel >= Preview.CurrentShip.Level)
        Preview.Result = EUpgradeResult::RequiresHigherShipLevel;
    else if (!UInventoryComponent::ProcessIngredients(Preview.Ingredients, GetInventories(), false))
        Preview.Result = EUpgradeResult::NotEnoughIngredients;
    if (bUpgradeInProgress) Preview.Result = EUpgradeResult::Busy;
    return Preview;
}

bool USpaceShipUpgradeComponent::HasEnoughIngredients(const TArray<FIngredient>& Ingredients,
    const TArray<UInventoryComponent*>& InInventories)
{
    return UInventoryComponent::ProcessIngredients(Ingredients, InInventories, false);
}

bool USpaceShipUpgradeComponent::TryConsumeIngredients(const TArray<FIngredient>& Ingredients,
    const TArray<UInventoryComponent*>& InInventories)
{
    return UInventoryComponent::ProcessIngredients(Ingredients, InInventories, true);
}

EUpgradeResult USpaceShipUpgradeComponent::TryUpgrade(EUpgradeTarget Target)
{
    if (bUpgradeInProgress) return EUpgradeResult::Busy;
    const FShipUpgradePreview Preview = GetPreview(Target);
    if (Preview.Result != EUpgradeResult::Success) return Preview.Result;
    ASpaceShipActor* Ship = Cast<ASpaceShipActor>(GetOwner());
    {
        TGuardValue<bool> Guard(bUpgradeInProgress, true);
        if (!TryConsumeIngredients(Preview.Ingredients, GetInventories()))
            return EUpgradeResult::NotEnoughIngredients;
        switch (Target)
        {
        case EUpgradeTarget::SpaceShip:
            Ship->ApplySpaceShipStat(Preview.NextShip);
            for (FName RecipeId : Preview.UnlockRecipeIds) CraftingComponent->Unlock(RecipeId);
            break;
        case EUpgradeTarget::Lazer: Ship->GetLazerComponent()->ApplyLazerStat(Preview.NextLazer); break;
        case EUpgradeTarget::MachineArm: Ship->GetMachineArmComponent()->ApplyMachineArmStat(Preview.NextArm); break;
        default: return EUpgradeResult::InvalidData;
        }
    }
    OnUpgradeStateChanged.Broadcast();
    return EUpgradeResult::Success;
}

bool USpaceShipUpgradeComponent::RestoreRecipeUnlocks()
{
    const ASpaceShipActor* Ship = Cast<ASpaceShipActor>(GetOwner());
    if (!Ship || !SpaceShipUpgradeTable || SpaceShipUpgradeTable->GetRowStruct() != FSpaceShipUpgrade::StaticStruct())
        return false;
    TArray<FSpaceShipUpgrade*> Rows;
    SpaceShipUpgradeTable->GetAllRows<FSpaceShipUpgrade>(TEXT("Restore ship recipes"), Rows);
    TArray<FName> Recipes;
    for (const auto* Row : Rows)
        if (Row && Row->Level >= 0 && Row->Level <= Ship->GetStat().Level)
            for (FName RecipeId : Row->UnlockRecipeIds)
            {
                if (!IsValid(CraftingComponent) || !CraftingComponent->HasRecipe(RecipeId)) return false;
                Recipes.AddUnique(RecipeId);
            }
    for (FName RecipeId : Recipes) CraftingComponent->Unlock(RecipeId);
    return true;
}

