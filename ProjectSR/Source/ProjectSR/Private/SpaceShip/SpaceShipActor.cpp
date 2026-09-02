// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/SpaceShipVisualActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/MachineArmComponent.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "SpaceShip/DoorButtonActor.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"
#include "MainPanel/MainPanelActor.h"

// Sets default values
ASpaceShipActor::ASpaceShipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent*	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SetRootComponent(RootSceneComponent);
	
	this->SpaceShipVisualActor_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpaceShipVisual"));
	this->SpaceShipVisualActor_->SetupAttachment(GetRootComponent());

	this->MainPanel_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("MainPanel"));
	this->MainPanel_->SetupAttachment(GetRootComponent());

	this->DoorButton_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("DoorButton"));
	this->DoorButton_->SetupAttachment(GetRootComponent());

	this->DoorMesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	this->DoorMesh_->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASpaceShipActor::BeginPlay()
{
	Super::BeginPlay();
	
	this->SpaceShipRotateState_ = FRotator::ZeroRotator;
	if (this->MainPanel_)
	{
		this->MainPanelActor_ = Cast<AMainPanelActor>(this->MainPanel_->GetChildActor());
		if (this->MainPanelActor_ == nullptr)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[ASpaceShipActor::BeginPlay] MainPanel의 실제 타입이 AMainPanelActor가 아닙니다.")
			);
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] MainPanel이 nullptr입니다.")
		);
	}
	if (this->DoorButton_)
	{
		this->DoorButtonActor_ = Cast<ADoorButtonActor>(this->DoorButton_->GetChildActor());
		if (this->DoorButtonActor_ == nullptr)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[ASpaceShipActor::BeginPlay] DoorButton의 실제 타입이 ADoorButtonActor가 아닙니다.")
			);
		}
		this->DoorButtonActor_->OnDoorButtonClick.BindUFunction(this, TEXT("DetectDoorButtonClick_"));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] DoorButton이 nullptr입니다.")
		);
	}
	if (this->DoorMesh_)
	{
		DoorTargetRotate_ = this->DoorMesh_->GetRelativeRotation();
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] DoorMesh가 nullptr입니다.")
		);
	}
	if (this->MeteorAvoidanceComponent_)
	{
		this->MeteorAvoidanceComponent_->SetSpaceShipSafeArea(this->SafeArea_);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] MeteorAvoidanceComponent가 nullptr입니다.")
		);
	}
}

// Called every frame
void ASpaceShipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UInventoryComponent* ASpaceShipActor::GetInventoryComponent_Implementation()
{
	return (this->WarehouseComponent_);
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

void ASpaceShipActor::DetectDoorButtonClick_()
{
	if (!this->DoorMesh_)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ADoorButtonActor::Interact_Implementation] DoorMesh가 설정되지 않았습니다.")
		);
		return ;
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ADoorButtonActor::Interact_Implementation] 문이 열리거나 닫힙니다.")
	);

	FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();

	this->DoorTargetRotate_.Roll = this->bIsDoorOpen_
			? this->DoorCloseAngle_
			: this->DoorOpenAngle_;
	this->bIsDoorOpen_ = !this->bIsDoorOpen_;
	GetWorldTimerManager().SetTimer(
		this->DoorMoveTimerHandle_,
		this,
		&ASpaceShipActor::UpdateDoorRotation_,
		this->DoorInteractTime_,
		true
	);
}

void	ASpaceShipActor::UpdateDoorRotation_()
{ 
	FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();
	FRotator	NowRotate = FMath::RInterpConstantTo(DoorRotation, this->DoorTargetRotate_, this->DoorInteractTime_, this->DoorRotationSpeed_);

	//UE_LOG(
	//	LogTemp,
	//	Log,
	//	TEXT("[ADoorButtonActor::Interact_Implementation] 문이 움직입니다.")
	//);
	this->DoorMesh_->SetRelativeRotation(NowRotate);
	if (FMath::IsNearlyEqual(NowRotate.Roll, this->DoorTargetRotate_.Roll))
	{
		//UE_LOG(
		//	LogTemp,
		//	Log,
		//	TEXT("[ADoorButtonActor::Interact_Implementation] 문이 움직입니다.")
		//);
		GetWorldTimerManager().ClearTimer(this->DoorMoveTimerHandle_);
	}
}

void	ASpaceShipActor::MeteorDetect(const USpaceMapDataAsset* InMapData)
{
	this->MeteorAvoidanceComponent_->MeteorDetect(InMapData);
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
