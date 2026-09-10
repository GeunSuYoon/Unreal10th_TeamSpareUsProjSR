#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SurvivalLoopActor.generated.h"

class ASpaceShipActor;
class APlayerCharacter;
class UBoxComponent;
class USpaceMapDataAsset;

UENUM(BlueprintType)
enum class ESurvivalState : uint8 { Ready, Playing, WaitingForMeteor, GameOver };

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival", meta = (ClampMin = "1.0"))
	float DayDuration = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival")
	bool bAutoStart = true;
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
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void BeginDay__();
	void UpdateGravity__();
	void EndGame__(ESurvivalEndReason Reason);
	UFUNCTION()
	void HandleDurability__(float Current, float Maximum);
};
