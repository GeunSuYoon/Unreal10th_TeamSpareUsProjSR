// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "Enum/MainPanelEnumDef.h"
#include "MainPanelActor.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMainPanelInteract, EMainPanelType, InMainPanelType);
DECLARE_DYNAMIC_DELEGATE(FOnMainPanelClose);

class ASpaceShipActor;
class UInventoryComponent;
class UCraftComponent;

UCLASS()
class PROJECTSR_API AMainPanelActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AMainPanelActor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void	Interact_Implementation(AActor* InTarget) override;

	void	ClosePanelWidget();
	void	OpenHomePanelWidget();
	void	OpenSpaceShipStatusPanelWidget();
	void	OpenWarehousePanelWidget();
	void	OpenSpaceShipControlPanelWidget();
	void	OpenSpaceShipUpgradePanelWidget();
	void	OpenItemCraftingPanelWidget();
	void	CloseSubPanelWidget();

	FOnMainPanelInteract	OnMainPanelInteract;
	FOnMainPanelClose		OnMainPanelClose;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent>	MainPanelMesh_ = nullptr;

private:
	TWeakObjectPtr<ASpaceShipActor>		SpaceShip__ = nullptr;
	TWeakObjectPtr<UInventoryComponent>	Warehouse__ = nullptr;
	TWeakObjectPtr<UCraftComponent>		Craft__ = nullptr;

	EMainPanelType	MainPanelState = EMainPanelType::None;
};
