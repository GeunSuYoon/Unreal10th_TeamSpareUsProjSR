// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Components/ActorComponent.h"
#include "MeteorAvoidanceComponent.generated.h"
//#include "Meteor.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMeteorDetect, const FMeteor&, InMeteor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMeteorMove, const FMeteor&, InMeteor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMeteorCollision, const FMeteor&, InMeteor);

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

	FOnMeteorDetect		OnMeteorDetect;
	FOnMeteorMove		OnMeteorMove;
	FOnMeteorCollision	OnMeteorCollision;

	void	SetSpaceShipSafeArea(const float InSafeArea) { this->SpaceShipSafeArea__ = InSafeArea; }

	void	MeteorDetect(const USpaceMapDataAsset* InSpaceMapData);
	void	SpaceShipMoveInput(const FVector2D& InMoveInput, const float InSpaceShipSpeed);
	void	EvaluateMeteorAvoidance();
	void	MeteorAlarm();
	void	SpawnMeteor();
	void	ClearMeteor();

protected:
	FMeteor	TargetMeteor_;

	//FVector	SpaceShipForward_;

private:
	//void	
	float	SpaceShipSafeArea__ = 500.0f;
	float	CollisionRadiusSquared__ = 0.0f;

	bool	bIsMeteor__ = false;
	bool	bIsAvoid__ = true;

	FTimerHandle	MeteorAlarmTimer__;
	float			MeteorAlarmTime__ = 1.0f;
};
