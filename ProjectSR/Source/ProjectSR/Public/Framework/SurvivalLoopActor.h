#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalLoopActor.generated.h"

class ASpaceShipActor;
class APlayerCharacter;
class UBoxComponent;
class USpaceMapDataAsset;
class APlayerController;

UENUM(BlueprintType)
enum class ESurvivalState : uint8 { Ready, Playing, WaitingForMeteor, GameOver, PreparingDay, PreparationFailed, Cleared };

UENUM(BlueprintType)
enum class ESurvivalEndReason : uint8 { ShipDestroyed, PlayerDied };

USTRUCT(BlueprintType)
struct FSurvivalMapEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 StartDay = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpaceMapDataAsset> MapData = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSurvivalDayStarted, int32, Day, USpaceMapDataAsset*, MapData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSurvivalGameOver, ESurvivalEndReason, Reason, int32, Day);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSurvivalGameCleared, int32, Day);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDayPreparationFailed, int32, Day, const FString&, Reason);

// Place one instance in the level. InteriorBounds must fit the cabin, not the meteor safe area.
UCLASS()
class PROJECTSR_API ASurvivalLoopActor : public AActor
{
	GENERATED_BODY()
public:
	ASurvivalLoopActor();
	virtual void Tick(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable, Category = "Survival")
	bool StartSurvival();
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void FinishDay();
	UFUNCTION(BlueprintCallable, Category = "Survival|Preparation")
	bool RetryDayPreparation();
	UFUNCTION(BlueprintPure, Category = "Survival|Preparation")
	float GetDayPreparationProgress() const;
	UFUNCTION(BlueprintPure, Category = "Survival")
	bool IsPlayerInside() const;
	UFUNCTION(BlueprintPure, Category = "Survival")
	bool IsPlayerSafe() const;
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void NotifyPlayerDeath();
	void NotifyMeteorImpact(ASpaceShipActor* HitShip);

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Survival")
	TObjectPtr<ASpaceShipActor> SpaceShip = nullptr;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Survival")
	TObjectPtr<APlayerCharacter> Player = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival")
	TObjectPtr<UBoxComponent> InteriorBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	TArray<FSurvivalMapEntry> Maps;
	// Independent day length. Oxygen capacity only limits how long the player can remain outside.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival", meta = (ClampMin = "1.0"))
	float DayDuration = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Daily", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LowEnergyOxygenRatio = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Daily", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DailyHealthRecoveryRatio = 0.5f;
	UPROPERTY(BlueprintReadOnly, Category = "Survival|Daily")
	bool bLastDailyEnergySufficient = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	bool bAutoStart = true;
	// The campaign is cleared after this day's settlement and solar-wind damage.
	// Set to 0 to run without a final day.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival", meta = (ClampMin = "0"))
	int32 FinalDay = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Preparation", meta = (ClampMin = "1.0"))
	float DayPreparationTimeout = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Preparation", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinimumInitialSpawnRatio = 0.8f;
	UPROPERTY(BlueprintReadOnly, Category = "Survival|Preparation")
	FString DayPreparationError;
	UPROPERTY(BlueprintAssignable, Category = "Survival|Preparation")
	FSurvivalDayStarted OnDayPreparationStarted;
	UPROPERTY(BlueprintAssignable, Category = "Survival|Preparation")
	FDayPreparationFailed OnDayPreparationFailed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind", meta = (ClampMin = "0.0"))
	float FirstSolarWindDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind", meta = (ClampMin = "0.0"))
	float SolarWindDamageIncrease = 5.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	int32 CurrentDay = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	float RemainingDayTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	ESurvivalState State = ESurvivalState::Ready;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	TObjectPtr<USpaceMapDataAsset> CurrentMap = nullptr;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalDayStarted OnDayStarted;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalGameOver OnGameOver;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalGameCleared OnGameCleared;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void BeginDay__();
	void CompleteDayPreparation__();
	void FailDayPreparation__(const FString& Reason);
	void SetPreparationInputLocked__(bool bLocked);
	bool bPreparationInputLocked__ = false;
	bool bPlayerInputWasEnabled__ = false;
	bool bMovementTickWasEnabled__ = false;
	TWeakObjectPtr<APlayerController> LockedController__;
	void ApplyDailySettlement__();
	void UpdateGravity__();
	void EndGame__(ESurvivalEndReason Reason);
	void ClearGame__();
	UFUNCTION()
	void HandleDurability__(float Current, float Maximum);
};
