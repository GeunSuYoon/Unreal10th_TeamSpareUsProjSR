// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/SpaceShipActor.h"
#include "MainPanel/MainPanelActor.h"

// Sets default values
ASpaceShipActor::ASpaceShipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpaceShipActor::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters Params;
	Params.Owner = this;

	this->MainPanel_ = GetWorld()->SpawnActor<AMainPanelActor>(
		AMainPanelActor::StaticClass(),
		GetActorTransform(),
		Params);

	if (this->MainPanel_)
	{
		this->MainPanel_->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

// Called every frame
void ASpaceShipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UInventoryComponent* ASpaceShipActor::GetInventoryComponent_Implementation()
{
	return (this->Warehouse_);
}

float ASpaceShipActor::RequestEnergy(float InEnergy)
{
	float	RetEnergy = FMath::Min(this->CurrentEnergy_, InEnergy);

	this->CurrentEnergy_ -= RetEnergy;
	return (RetEnergy);
}

void ASpaceShipActor::RepairDurability_Implementation(float InDurability)
{
	this->CurrentDurability_ = FMath::Min(this->CurrentDurability_ + InDurability, this->MaxDurability_);
}

void ASpaceShipActor::ConsumDurability_Implementation(float InDurability)
{
	this->CurrentDurability_ = FMath::Min(this->CurrentDurability_ - InDurability, 0.0f);
}

