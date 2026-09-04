// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MachineArmComponent.h"
#include "DataAsset/MachineArmDataAsset.h"

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
		this->ItemCollectTime__ = InMachineArmData->ItemCollectTime;
		this->ItemCollectWeight__ = InMachineArmData->ItemCollectWeight;
		this->ReactiveEnergy__ = InMachineArmData->ReactiveEnergy;
	}
	else
	{
		this->ItemCollectTime__ = 0.0f;
		this->ItemCollectWeight__ = 0.0f;
		this->ReactiveEnergy__ = 0.0f;
	}
}

