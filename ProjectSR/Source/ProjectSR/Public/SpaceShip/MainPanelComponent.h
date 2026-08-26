// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/InteractInterface.h"
#include "Enum/MainPanelEnumDef.h"
#include "MainPanelComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnMainPanelInteract);

class ASpaceShipActor;
class UInventoryComponent;
class UCraftComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UMainPanelComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMainPanelComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void	Interact_Implementation(AActor* InTarget) override;

	FOnMainPanelInteract	OnMainPanelInteract;

private:
	TWeakObjectPtr<ASpaceShipActor>		SpaceShip__ = nullptr;
	TWeakObjectPtr<UInventoryComponent>	Warehouse__ = nullptr;
	TWeakObjectPtr<UCraftComponent>		Craft__ = nullptr;

	EMainPanelType	MainPanelState = EMainPanelType::None;
};
