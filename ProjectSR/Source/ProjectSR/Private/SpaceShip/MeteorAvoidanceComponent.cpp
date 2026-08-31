// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Data/SpaceMap/SpaceMapDataAsset.h"

// Sets default values for this component's properties
UMeteorAvoidanceComponent::UMeteorAvoidanceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMeteorAvoidanceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UMeteorAvoidanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMeteorAvoidanceComponent::MeteorDetect(const USpaceMapDataAsset* InSpaceMapData)
{
	if (!InSpaceMapData)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UMeteorAvoidanceComponent::MeteorDetect] InSpaceMapData가 nullptr입니다.")
		);
		return ;
	}
	if (InSpaceMapData->MeteorAlarmTimer <= 0.0f
		|| InSpaceMapData->MeteorSpawnLeadTime < 0.0f
		|| InSpaceMapData->MeteorSpawnLeadTime >= InSpaceMapData->MeteorAlarmTimer
		|| InSpaceMapData->MeteorSpeed <= 0.0f)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UMeteorAvoidanceComponent::MeteorDetect] InSpaceMapData [%s]의 값이 이상합니다."),
			*InSpaceMapData->MapName.ToString()
		);
		return ;
	}
	this->TargetMeteor_.MeteorRemainTime = InSpaceMapData->MeteorAlarmTimer;
	this->TargetMeteor_.MeteorSpawnLeadTime = InSpaceMapData->MeteorSpawnLeadTime;
	this->TargetMeteor_.MeteorDamage = InSpaceMapData->MeteorDamage;
	this->TargetMeteor_.MeteorSpeed = InSpaceMapData->MeteorSpeed;
	this->TargetMeteor_.MeteorSize = InSpaceMapData->MeteorSize;
	this->TargetMeteor_.MoveDir = FVector(
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f)
	);
	this->TargetMeteor_.MoveDir.Normalize();
	this->TargetMeteor_.SpawnPos = -1.0f * this->TargetMeteor_.MoveDir * InSpaceMapData->ItemSpawnDist;
	this->TargetMeteor_.DespawnPos = this->TargetMeteor_.MoveDir * InSpaceMapData->ItemSpawnDist;
	this->bIsMeteor__ = true;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		this->MeteorAlarmTimer__,
		this,
		&UMeteorAvoidanceComponent::MeteorAlarm,
		this->MeteorAlarmTime__,
		true,
		0.0f
	);
}

void UMeteorAvoidanceComponent::SpaceShipMoveInput(const FVector2D& InMoveInput, const float InSpaceShipSpeed)
{
	FVector2D	MeteorMoveDir = FVector2D(-InMoveInput.Y, -InMoveInput.X);


	OnMeteorAvoid.ExecuteIfBound(this->TargetMeteor_);
}

void UMeteorAvoidanceComponent::EvaluateMeteorAvoidance()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.ClearTimer(this->MeteorAlarmTimer__);
}

void UMeteorAvoidanceComponent::MeteorAlarm()
{
	this->TargetMeteor_.MeteorRemainTime -= this->MeteorAlarmTime__;
	if (this->TargetMeteor_.MeteorRemainTime <= this->TargetMeteor_.MeteorSpawnLeadTime)
	{
		this->EvaluateMeteorAvoidance();
		return ;
	}
	OnMeteorDetect.ExecuteIfBound();
}

void UMeteorAvoidanceComponent::ClearMeteor()
{
	this->TargetMeteor_.Clear();
	this->bIsMeteor__ = false;
}
