// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/DoorButtonActor.h"
#include "Components/SphereComponent.h"

// Sets default values
ADoorButtonActor::ADoorButtonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->InteractSphere_ = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	this->InteractSphere_->SetSphereRadius(this->InteractSphereRadius_);
	SetRootComponent(this->InteractSphere_);

	this->ButtonMesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	this->ButtonMesh_->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ADoorButtonActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorButtonActor::Interact_Implementation(AActor* InTarget)
{
	//if (!this->DoorMesh_)
	//{
	//	UE_LOG(
	//		LogTemp,
	//		Error,
	//		TEXT("[ADoorButtonActor::Interact_Implementation] DoorMesh가 설정되지 않았습니다.")
	//	);
	//	return ;
	//}

	//FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();

	//this->TargetRotation_.Pitch = this->bIsOpen_
	//		? -this->DoorOpenAngle_
	//		: this->DoorOpenAngle_;
	//this->bIsOpen_ = !this->bIsOpen_;
	//GetWorldTimerManager().SetTimer(
	//	this->DoorMoveTimerHandle_,
	//	this,
	//	&ADoorButtonActor::UpdateDoorRotation,
	//	this->DoorMoveTime_,
	//	true
	//);
}

//void ADoorButtonActor::UpdateDoorRotation()
//{
//	FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();
//	FRotator	NowRotate = FMath::RInterpConstantTo(DoorRotation, this->TargetRotation_, GetWorld()->DeltaRealTimeSeconds, this->DoorMoveTime_);
//
//	this->DoorMesh_->SetRelativeRotation(NowRotate);
//	if (FMath::IsNearlyEqual(NowRotate.Pitch, this->TargetRotation_.Pitch))
//	{
//		GetWorldTimerManager().ClearTimer(this->DoorMoveTimerHandle_);
//	}
//}

