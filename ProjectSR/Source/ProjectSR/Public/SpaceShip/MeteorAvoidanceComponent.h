// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeteorAvoidanceComponent.generated.h"
//#include "Meteor.h"

USTRUCT(BlueprintType)
struct FMeteor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorRemainTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSpawnLeadTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	MoveDir = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	SpawnPos = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	DespawnPos = FVector::Zero();

	void	Clear()
	{
		MeteorRemainTime = 0.0f;
		MeteorSpawnLeadTime = 0.0f;
		MeteorDamage = 0.0f;
		MeteorSpeed = 0.0f;
		MeteorSize = 0.0f;
		MoveDir = FVector::Zero();
		SpawnPos = FVector::Zero();
		DespawnPos = FVector::Zero();
	}
};

DECLARE_DYNAMIC_DELEGATE(FOnMeteorDetect);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMeteorAvoid, const FMeteor&, InMeteor);

class USpaceMapDataAsset;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSR_API UMeteorAvoidanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeteorAvoidanceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnMeteorDetect	OnMeteorDetect;
	FOnMeteorAvoid	OnMeteorAvoid;

	void	MeteorDetect(const USpaceMapDataAsset* InSpaceMapData);
	void	SpaceShipMoveInput(const FVector2D& InMoveInput, const float InSpaceShipSpeed);
	void	EvaluateMeteorAvoidance();
	void	MeteorAlarm();
	void	ClearMeteor();

protected:
	FMeteor	TargetMeteor_;

	//FVector	SpaceShipForward_;

private:
	//void	
	bool	bIsMeteor__ = false;
	bool	bIsAvoid__ = false;

	FTimerHandle	MeteorAlarmTimer__;
	float			MeteorAlarmTime__ = 1.0f;
};