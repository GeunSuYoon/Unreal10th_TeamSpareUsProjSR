// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/MainPanelUserWidget.h"
#include "Widget/MainPanel/MainPanelHomeUserWidget.h"
#include "Widget/MainPanel/Status/SpaceShipActorStatusUserWidget.h"
#include "Widget/ItemManagerWidget.h"
#include "Widget/MeteorEventUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgaradeMainUserWidget.h"
#include "Widget/Crafting/RecipeListWidget.h"

#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "Interface/InventoryComponentInterface.h"

#include "MainPanel/MainPanelActor.h"

#include "GameFramework/Pawn.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"

bool UMainPanelUserWidget::HandleMeteorMoveKey(const FKeyEvent& InKeyEvent)
{
	return MainPanelSwitcher && MeteoEvent
		&& MainPanelSwitcher->GetActiveWidget() == MeteoEvent.Get()
		&& MeteoEvent->HandleMoveKey(InKeyEvent);
}

void	UMainPanelUserWidget::OpenSelfWidget_Implementation()
{
	if (SpaceShipUpgrade) SpaceShipUpgrade->RefreshUpgradePages();
	this->SetVisibility(ESlateVisibility::Visible);
	OnWidgetOpen.ExecuteIfBound(this);
}

void	UMainPanelUserWidget::CloseSelfWidget_Implementation()
{
	IWidgetStackHostInterface::Execute_ClearStackWidget(this);
	this->SetVisibility(ESlateVisibility::Collapsed);
	OnWidgetClose.ExecuteIfBound(this);
}

bool	UMainPanelUserWidget::CloseTopWidget_Implementation()
{
	if (!ensure(this->MainPanelSwitcher))
	{
		return (true);
	}
	if (this->OpenWidgetStack__.Num() <= 1)
	{
		return (true);
	}
	if (this->OpenWidgetStack__.Last()->GetClass()->ImplementsInterface(UWidgetStackHostInterface::StaticClass()))
	{
		if (!IWidgetStackHostInterface::Execute_CloseTopWidget(this->OpenWidgetStack__.Last()))
			return (false);
	}
	if (this->OpenWidgetStack__.Last() == this->Warehouse)
	{
		this->Warehouse->OnItemManagerClose.ExecuteIfBound();
	}
	if (this->OpenWidgetStack__.Last() == this->CraftingRecipeList)
	{
		this->CraftingRecipeList->OnCraftingClose.ExecuteIfBound();
	}
	//this->StackSize__--;
	this->OpenWidgetStack__.Pop();
	this->SwitchWidget(this->OpenWidgetStack__.Num() - 1);
	return (false);
}

void UMainPanelUserWidget::ClearStackWidget_Implementation()
{
	while (!IWidgetStackHostInterface::Execute_CloseTopWidget(this))
	{	}
}

void UMainPanelUserWidget::OpenMainPanel()
{
	IOpenableWidgetInterface::Execute_OpenSelfWidget(this);
}

void	UMainPanelUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	if (!IsValid(InSpaceShip)) return;
	if (USpaceShipUpgradeComponent* Upgrade = InSpaceShip->GetUpgradeComponent())
	{
		// Match the manufacture screen: ingredients can be paid from both the
		// player's inventory and the ship warehouse.
		// Explicitly configured inventory sources are preserved.
		APawn* Pawn = GetOwningPlayerPawn();
		if (!Upgrade->HasConfiguredInventories())
		{
			TArray<UInventoryComponent*> Sources;
			if (Pawn && Pawn->GetClass()->ImplementsInterface(UInventoryComponentInterface::StaticClass()))
			{
				Sources.Add(IInventoryComponentInterface::Execute_GetInventoryComponent(Pawn));
			}
			Sources.Add(InSpaceShip->GetWarehouse());
			Upgrade->SetInventories(Sources);
		}
		if (SpaceShipUpgrade) SpaceShipUpgrade->BindToUpgradeComponent(Upgrade);
	}
	this->SpaceShipStatus->BindToSpaceShip(InSpaceShip);
	this->MeteoEvent->BindToSpaceShip(InSpaceShip);
	this->Warehouse->BindToInventoryComponent(InSpaceShip->GetWarehouse());
	//this->Warehouse->InitializeInventoryWidget();
	this->CraftingRecipeList->BindToCraftingComponent(InSpaceShip->GetCraftingComponent());
	if (AMainPanelActor* MainPanelActor = InSpaceShip->GetMainPanelActor())
	{
		MainPanelActor->OnMainPanelActorInteract.BindUFunction(this, TEXT("OpenMainPanel"));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::BindToSpaceShip] MainPanelActor가 nullptr입니다.")
		);
	}
}

void UMainPanelUserWidget::SwitchWidget(int32 InIndex)
{
	if (!this->MainPanelSwitcher ||
		InIndex >= MainPanelSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::SwitchWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	MainPanelSwitcher->SetActiveWidgetIndex(InIndex);
}

void UMainPanelUserWidget::SwitchTargetWidget(EMainPanelMenuPage InPage)
{
	int32	InIndex = static_cast<int32>(InPage);

	if (!this->MainPanelSwitcher ||
		InIndex >= MainPanelSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::SwitchTargetWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	if (InPage == EMainPanelMenuPage::Warehouse)
	{
		this->Warehouse->OnItemManagerOpen.ExecuteIfBound();
	}
	this->MainPanelSwitcher->SetActiveWidgetIndex(InIndex);
	this->OpenWidgetStack__.Add(MainPanelSwitcher->GetWidgetAtIndex(static_cast<int32>(InPage)));
}

void UMainPanelUserWidget::CloseDetect()
{
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}

void UMainPanelUserWidget::BackspaceDetect()
{
	IWidgetStackHostInterface::Execute_CloseTopWidget(this);
}

void	UMainPanelUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(this->MainPanelSwitcher && this->MainPanelHome))
	{
		this->SwitchTargetWidget(EMainPanelMenuPage::Home);
		this->MainPanelHome->OnMainPanelHomeSelect.BindUFunction(this, TEXT("SwitchTargetWidget"));
	}
	this->BackSpaceButton->OnClicked.AddDynamic(this, &UMainPanelUserWidget::BackspaceDetect);
	this->CloseButton->OnClicked.AddDynamic(this, &UMainPanelUserWidget::CloseDetect);
}
