// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MeteorAvoidanceComponent.h"

// Sets default values for this component's properties
UMeteorAvoidanceComponent::UMeteorAvoidanceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

void UMeteorAvoidanceComponent::MeteorDetect(const FMeteor& InMeteor)
{
	this->TargetMeteorList_.Add(InMeteor);
	//this->TargetMeteor_ = InMeteor;
}

void UMeteorAvoidanceComponent::SpaceShipRotateChange(const FRotator& InSpaceShipRotate)
{
	this->SpaceShipRotate_ = InSpaceShipRotate;
}

void UMeteorAvoidanceComponent::EvaluateMeteorAvoidance()
{
}
