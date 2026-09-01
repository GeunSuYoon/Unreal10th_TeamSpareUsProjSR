// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/SpaceShipVisualActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/MachineArmComponent.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Component/InventoryComponent.h"
#include "MainPanel/MainPanelActor.h"

// Sets default values
ASpaceShipActor::ASpaceShipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent*	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SetRootComponent(RootSceneComponent);
	
	this->SpaceShipVisualActor_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpaceShipVisual"));
	this->SpaceShipVisualActor_->SetupAttachment(RootSceneComponent);
}

// Called when the game starts or when spawned
void ASpaceShipActor::BeginPlay()
{
	Super::BeginPlay();
	
	this->SpaceShipRotateState_ = FRotator::ZeroRotator;
	FActorSpawnParameters Params;
	Params.Owner = this;
	this->MainPanel_ = GetWorld()->SpawnActor<AMainPanelActor>(
		AMainPanelActor::StaticClass(),
		GetActorTransform(),
		Params
	);
	if (this->MainPanel_)
	{
		this->MainPanel_->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		this->MainPanel_->OnSpaceShipRotateInput.BindUFunction(this, TEXT("SpaceShipRotateInput_"));
	}
	if (this->MeteorAvoidance_)
	{
		this->MeteorAvoidance_->SetSpaceShipSafeArea(this->SafeArea_);
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
	this->CurrentDurability_ = FMath::Max(this->CurrentDurability_ + InDurability, this->MaxDurability_);
}

void ASpaceShipActor::ConsumDurability_Implementation(float InDurability)
{
	this->CurrentDurability_ = FMath::Min(this->CurrentDurability_ - InDurability, 0.0f);
}

void	ASpaceShipActor::MeteorDetect(const USpaceMapDataAsset* InMapData)
{
	this->MeteorAvoidance_->MeteorDetect(InMapData);
}

//void ASpaceShipActor::SpaceShipRotateInput_(const FVector2D& InInput)
//{
//	float	DeltaTime = GetWorld()->GetDeltaSeconds();
//
//	FVector2D	InputToRotate = InInput.GetClampedToMaxSize(1.0f) * this->RotateSpeed_ * DeltaTime;
//
//	this->SpaceShipRotateState_ += FRotator(
//		-InputToRotate.Y,	// Pitch
//		-InputToRotate.X,	// Yaw
//		0.0f				// Roll
//	);
//	this->OnSpaceShipRotate.ExecuteIfBound(this->SpaceShipRotateState_);
//	//AddActorLocalRotation(DeltaRotation);
//}
