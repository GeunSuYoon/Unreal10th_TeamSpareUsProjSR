#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CommonHeader/SpaceShipStruct.h"
#include "SurvivalSaveGame.generated.h"

class UItemDataAsset;

USTRUCT()
struct FInventorySlotSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TSoftObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY(SaveGame)
	int32 Count = 0;
};

UCLASS()
class PROJECTSR_API USurvivalSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static constexpr int32 CurrentSaveVersion = 1;

	UPROPERTY(SaveGame)
	int32 SaveVersion = CurrentSaveVersion;

	// The last fully resolved day. Loading starts preparation for CompletedDay + 1.
	UPROPERTY(SaveGame)
	int32 CompletedDay = 0;

	UPROPERTY(SaveGame)
	float PlayerHealth = 0.0f;

	UPROPERTY(SaveGame)
	float PlayerOxygen = 0.0f;

	UPROPERTY(SaveGame)
	FSpaceShipStat SpaceShipStat;

	UPROPERTY(SaveGame)
	FLazerStat LazerStat;

	UPROPERTY(SaveGame)
	float SpaceShipDurability = 0.0f;

	UPROPERTY(SaveGame)
	float SpaceShipEnergy = 0.0f;

	UPROPERTY(SaveGame)
	int32 PlayerInventorySize = 0;

	UPROPERTY(SaveGame)
	TArray<FInventorySlotSaveData> PlayerInventory;

	UPROPERTY(SaveGame)
	int32 WarehouseSize = 0;

	UPROPERTY(SaveGame)
	TArray<FInventorySlotSaveData> Warehouse;
};
