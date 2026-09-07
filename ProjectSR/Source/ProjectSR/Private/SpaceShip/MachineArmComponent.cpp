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
	
}


// Called every frame
void UMachineArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMachineArmComponent::SetMachineArmData(UMachineArmDataAsset* InMachineArmData)
{
	this->MachineArmData__ = InMachineArmData;
	if (InMachineArmData)
	{
		this->MachineArmStat__.Level = InMachineArmData->MachineArmStat.Level;
		this->MachineArmStat__.ItemCollectTime = InMachineArmData->MachineArmStat.ItemCollectTime;
		this->MachineArmStat__.ItemCollectWeight = InMachineArmData->MachineArmStat.ItemCollectWeight;
		this->MachineArmStat__.OperationalEnergy = InMachineArmData->MachineArmStat.OperationalEnergy;
	}
	else
	{
		this->MachineArmStat__.Level = 0;
		this->MachineArmStat__.ItemCollectTime = 0.0f;
		this->MachineArmStat__.ItemCollectWeight = 0.0f;
		this->MachineArmStat__.OperationalEnergy = 0.0f;
	}
	OnMachineArmLevelChange.ExecuteIfBound(this->MachineArmStat__);
}

