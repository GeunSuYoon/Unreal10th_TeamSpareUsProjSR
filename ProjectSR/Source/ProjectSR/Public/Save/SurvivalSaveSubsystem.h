#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SurvivalSaveSubsystem.generated.h"

class ASurvivalLoopActor;
class UInventoryComponent;
class USurvivalSaveGame;

UCLASS()
class PROJECTSR_API USurvivalSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Save")
	bool HasSaveGame() const;

	// Called by the main menu before travelling to the gameplay map.
	void RequestLoad() { bLoadRequested = true; }

	// Called by SurvivalLoop before the first BeginDay. Consumes the request only on success.
	bool TryApplyRequestedLoad(ASurvivalLoopActor* SurvivalLoop);

	// Writes a checkpoint only after the current day's meteor resolution is complete.
	bool SaveCompletedDay(const ASurvivalLoopActor* SurvivalLoop) const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool DeleteSaveGame();

private:
	static constexpr int32 UserIndex = 0;
	static const FString SlotName;
	bool bLoadRequested = false;

	static void CaptureInventory(const UInventoryComponent* Inventory, int32& OutSize,
		TArray<struct FInventorySlotSaveData>& OutSlots);
	static bool ResolveInventory(int32 SavedSize, const TArray<struct FInventorySlotSaveData>& SavedSlots,
		TArray<struct FInventorySlot>& OutSlots);
};
