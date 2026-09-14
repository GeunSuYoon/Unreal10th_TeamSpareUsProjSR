// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPanel/MainPanelActor.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Data/SpaceShip/SpaceShipDataAsset.h"
#include "Data/Item/ItemDataAsset.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
AMainPanelActor::AMainPanelActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = false;

	// ...
	//USceneComponent*	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));


	this->SphereCollision_ = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(this->SphereCollision_);

	this->MainPanelMesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainPanelMesh"));
	this->MainPanelMesh_->SetupAttachment(GetRootComponent());

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidgetComponent->SetupAttachment(GetRootComponent());
	InteractionWidgetComponent->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidgetComponent->SetVisibility(false);
	InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts
void AMainPanelActor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (ASpaceShipActor* OwnerSpaceShip = Cast<ASpaceShipActor>(this->GetOwner()))
	{
		this->SpaceShip__ = OwnerSpaceShip;
		// TODO: InventoryComponent 만들면 거기랑 연결해야해용
		//this->Warehouse__ = IInventoryComponentInterface::Execute_GetInventoryComponent(OwnerSpaceShip);
	}
}

// Called every frame
void AMainPanelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainPanelActor::Interact_Implementation(AActor* InTarget)
{
	OnMainPanelActorInteract.ExecuteIfBound();
}

void AMainPanelActor::OnFocused_Implementation(AActor* InTarget)
{
	if (!InteractionWidgetComponent)
	{
		return;
	}

	InteractionWidgetComponent->SetVisibility(true);
	SetActorTickEnabled(true);
	//UpdateInteractionWidgetFacing__();
}

void AMainPanelActor::OnUnfocused_Implementation(AActor* InTarget)
{
	if (!InteractionWidgetComponent)
	{
		return;
	}

	InteractionWidgetComponent->SetVisibility(false);
	SetActorTickEnabled(false);
	UpdateInteractionWidgetFacing__();
}

void AMainPanelActor::UpdateInteractionWidgetFacing__()
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

