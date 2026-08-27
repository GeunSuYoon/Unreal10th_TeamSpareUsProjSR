// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPanel/MainPanelActor.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Data/SpaceShip/SpaceShipUpgradeDataTable.h"
#include "Data/Item/ItemDataAsset.h"

// Sets default values for this component's properties
AMainPanelActor::AMainPanelActor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = false;

	// ...
	this->MainPanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainPanelMesh"));
	this->SetRootComponent(this->MainPanelMesh);
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
	OnMainPanelInteract.ExecuteIfBound();
}

