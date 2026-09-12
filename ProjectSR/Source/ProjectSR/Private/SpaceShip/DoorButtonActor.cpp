// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/DoorButtonActor.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidgetComponent->SetupAttachment(GetRootComponent());
	InteractionWidgetComponent->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ADoorButtonActor::Interact_Implementation] DoorButtonActor가 상호작용을 시작했습니다.")
	);
	OnDoorButtonClick.ExecuteIfBound();
}

void ADoorButtonActor::OnFocused_Implementation(AActor* InTarget)
{
	if (!InteractionWidgetComponent)
	{
		return;
	}

	InteractionWidgetComponent->SetVisibility(true);
	SetActorTickEnabled(true);
	UpdateInteractionWidgetFacing__();
}

void ADoorButtonActor::OnUnfocused_Implementation(AActor* InTarget)
{
	if (!InteractionWidgetComponent)
	{
		return;
	}

	InteractionWidgetComponent->SetVisibility(false);
	SetActorTickEnabled(false);
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


void ADoorButtonActor::UpdateInteractionWidgetFacing__()
{
	if (!InteractionWidgetComponent)
	{
		return;
	}

	APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CamManager)
	{
		return;
	}

	const FVector CameraLoc = CamManager->GetCameraLocation();
	const FVector ObjectLoc = GetActorLocation();

	const FVector Dir = (CameraLoc - ObjectLoc).GetSafeNormal();
	const FVector NewWidgetLoc = ObjectLoc + Dir * WidgetOffsetDistance__;

	InteractionWidgetComponent->SetWorldLocation(NewWidgetLoc);
	InteractionWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(NewWidgetLoc, CameraLoc));
}

