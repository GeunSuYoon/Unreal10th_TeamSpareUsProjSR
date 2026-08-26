// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MainPanelComponent.h"
#include "SpaceShip/SpaceShipActor.h"

// Sets default values for this component's properties
UMainPanelComponent::UMainPanelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

}


// Called when the game starts
void UMainPanelComponent::BeginPlay()
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
void UMainPanelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMainPanelComponent::Interact_Implementation(AActor* InTarget)
{
	OnMainPanelInteract.ExecuteIfBound();
}

