// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MachineArmComponent.h"
#include "Data/SpaceShip/MachineArmDataAsset.h"

// Sets default values for this component's properties
UMachineArmComponent::UMachineArmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMachineArmComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// MachineArm feature retired.
	// OnMachineArmLevelChange.ExecuteIfBound(this->MachineArmStat__);
}


// Called every frame
void UMachineArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMachineArmComponent::SetMachineArmData(UMachineArmDataAsset* InMachineArmData)
{
	// MachineArm feature retired. Keep this no-op function so legacy callers still link.
	/*
	this->MachineArmData__ = InMachineArmData;
	if (InMachineArmData)
	{
		this->MachineArmStat__ = InMachineArmData->MachineArmStat;
	}
	OnMachineArmLevelChange.ExecuteIfBound(this->MachineArmStat__);
	*/
}

