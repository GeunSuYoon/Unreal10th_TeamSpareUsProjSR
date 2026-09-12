#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalLoopActor.generated.h"

class ASpaceShipActor;
class APlayerCharacter;
class UBoxComponent;
class USpaceMapDataAsset;
class APlayerController;
class UUserWidget;
class USoundBase;
class UCameraShakeBase;

UENUM(BlueprintType)
enum class ESurvivalState : uint8 { Ready, Playing, WaitingForMeteor, GameOver, Cleared };

UENUM(BlueprintType)
enum class ESurvivalEndReason : uint8
{
	ShipDestroyed,
	PlayerDied,
	MeteorCollision,
	SolarWind
};

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSurvivalDayStatusChanged, int32, Day,
	int32, DaysUntilSolarWind, float, SolarWindDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSurvivalGameOver, ESurvivalEndReason, Reason, int32, Day);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSurvivalGameCleared, int32, Day);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequiredDurabilityChanged, float, RequiredDurability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSurvivalFadeEvent);

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
	UFUNCTION(BlueprintPure, Category = "Survival")
	bool IsPlayerInside() const;
	UFUNCTION(BlueprintPure, Category = "Survival")
	bool IsPlayerSafe() const;
	UFUNCTION(BlueprintCallable, Category = "Survival")
	void NotifyPlayerDeath();
	void NotifyMeteorImpact(ASpaceShipActor* HitShip, float Damage = 0.0f);

	void	InitDelegate();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Survival")
	TObjectPtr<ASpaceShipActor> SpaceShip = nullptr;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Survival")
	TObjectPtr<APlayerCharacter> Player = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Survival")
	TObjectPtr<UBoxComponent> InteriorBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	TArray<FSurvivalMapEntry> Maps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Daily", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LowEnergyOxygenRatio = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Daily", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DailyHealthRecoveryRatio = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Transition", meta = (ClampMin = "0.0"))
	float DayFadeOutDuration = 0.75f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|Transition", meta = (ClampMin = "0.0"))
	float DayFadeInDuration = 0.75f;
	UPROPERTY(BlueprintReadOnly, Category = "Survival|Daily")
	bool bLastDailyEnergySufficient = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	bool bAutoStart = true;
	// The campaign is cleared after this day's settlement and solar-wind damage.
	// Set to 0 to run without a final day.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival", meta = (ClampMin = "0"))
	int32 FinalDay = 12;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind", meta = (ClampMin = "0.0"))
	float FirstSolarWindDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind", meta = (ClampMin = "0.0"))
	float SolarWindDamageIncrease = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind")
	TObjectPtr<USoundBase> SolarWindSFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|SolarWind")
	TSubclassOf<UCameraShakeBase> SolarWindCameraShake;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	int32 CurrentDay = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	ESurvivalState State = ESurvivalState::Ready;
	UPROPERTY(BlueprintReadOnly, Category = "Survival")
	TObjectPtr<USpaceMapDataAsset> CurrentMap = nullptr;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalDayStarted OnDayStarted;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalDayStatusChanged OnDayStatusChanged;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalGameOver OnGameOver;
	UPROPERTY(BlueprintAssignable, Category = "Survival")
	FSurvivalGameCleared OnGameCleared;
	UPROPERTY(BlueprintAssignable, Category = "Survival|SolarWind")
	FRequiredDurabilityChanged OnRequiredDurabilityChanged;
	UPROPERTY(BlueprintAssignable, Category = "Survival|Transition")
	FSurvivalFadeEvent OnDayFadeOut;
	UPROPERTY(BlueprintAssignable, Category = "Survival|Transition")
	FSurvivalFadeEvent OnDayFadeIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|GameOver")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Survival|GameClear")
	TSubclassOf<UUserWidget> GameClearWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival|GameOver")
	FName MainMenuLevel = TEXT("/Game/FirstPerson/Lvl_FirstPerson");
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void BeginDay__();
	void HandleDayFadeOutComplete__();
	void StartDayFadeIn__();
	void HandleDayFadeInComplete__();
	void EnterPlayableDay__();
	void RecenterPlayer__();
	void SetTransitionInputLocked__(bool bLocked);
	bool bTransitionInputLocked__ = false;
	bool bPlayerInputWasEnabled__ = false;
	bool bMovementTickWasEnabled__ = false;
	TWeakObjectPtr<APlayerController> LockedController__;
	FTimerHandle DayFadeInTimer__;
	float DayFadeOutRemaining__ = 0.0f;
	bool bTransitionFromCompletedDay__ = false;
	void ApplyDailySettlement__();
	int32 GetNextSolarWindDay__() const;
	float GetNextSolarWindDamage__() const;
	void UpdateGravity__();
	void EndGame__(ESurvivalEndReason Reason);
	void ClearGame__();
	void ShowResultWidget__(bool bGameCleared, ESurvivalEndReason Reason = ESurvivalEndReason::PlayerDied);
	UFUNCTION()
	void HandleGoToMainMenu__();

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> GameOverWidgetInstance__;
	ESurvivalEndReason PendingPlayerDeathReason__ = ESurvivalEndReason::PlayerDied;
	ESurvivalEndReason PendingShipDestructionReason__ = ESurvivalEndReason::ShipDestroyed;

	UFUNCTION()
	void HandleDurability__(float Current, float Maximum);
};
