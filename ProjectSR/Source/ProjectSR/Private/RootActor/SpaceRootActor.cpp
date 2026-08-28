// Fill out your copyright notice in the Description page of Project Settings.


#include "RootActor/SpaceRootActor.h"

// Sets default values
ASpaceRootActor::ASpaceRootActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->RootScene_ = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(this->RootComponent);
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);

	this->BackgroundPivot_ = CreateDefaultSubobject<USceneComponent>(TEXT("BackgroundPivot"));
	this->BackgroundPivot_->SetupAttachment(this->RootScene_);

	this->ItemPivot_ = CreateDefaultSubobject<USceneComponent>(TEXT("ItemPivot"));
	this->ItemPivot_->SetupAttachment(this->RootScene_);

	this->SpawnShellPivot_ = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnShellPivot"));
	this->SpawnShellPivot_->SetupAttachment(this->RootScene_);

	this->MeteorPivot_ = CreateDefaultSubobject<USceneComponent>(TEXT("MeteorPivot"));
	this->MeteorPivot_->SetupAttachment(this->RootScene_);
}

// Called when the game starts or when spawned
void ASpaceRootActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASpaceRootActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void ASpaceRootActor::RotateSpaceRoot(const FVector2D& InInput, const float InRotateSpeed)
//{
//	float	DeltaTime = GetWorld()->GetDeltaSeconds();
//
//	FVector2D	InputToRotate = InInput.GetClampedToMaxSize(1.0f) * InRotateSpeed * DeltaTime;
//
//	FQuat DeltaRotation = FRotator(
//		-InputToRotate.Y,	// Pitch
//		-InputToRotate.X,	// Yaw
//		0.0f				// Roll
//	).Quaternion();
//
//	AddActorLocalRotation(DeltaRotation);
//}

void ASpaceRootActor::RotateSpaceRoot(const FRotator& InRotate)
{
	this->SetActorRotation(InRotate);
}
