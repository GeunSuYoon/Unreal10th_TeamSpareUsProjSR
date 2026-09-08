// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainUserWidget.h"
#include "Widget/MeteorWarningUserWidget.h"
#include "Widget/InventoryWindowWidget.h"
#include "Widget/MainPanel/MainPanelUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Widget/Crafting/RecipeListWidget.h"
#include "Widget/Crafting/ManufactureWidget.h"
#include "Interface/InventoryComponentInterface.h"
#include "Player/PlayerCharacter.h"
#include "Interface/OpenableWidgetInterface.h"

void UMainUserWidget::BindToPlayer(APlayerCharacter* InPlayerCharacter)
{
    if (!InPlayerCharacter)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("[UMainUserWidget::BindToStorage] InPlayerCharacter가 nullptr입니다.")
        );
        return;
    }
    //this->InventoryWindow->BindToInventoryComponent(IInventoryComponentInterface::Execute_GetInventoryComponent(InPlayerCharacter));
    //InPlayerCharacter->OnToggleInventory.BindUFunction(InventoryWindow, TEXT("ToggleInventoryWidget"));
}

void UMainUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShipActor)
{
	if (!InSpaceShipActor)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainUserWidget::BindToSpaceShip] InSpaceShipActor가 nullptr입니다.")
		);
		return;
	}
	this->MeteoWarningWidget->SetVisibility(ESlateVisibility::Collapsed);
	this->MeteoWarningWidget->BindToMeteorAvoidanceComponent(InSpaceShipActor->GetMeteorAvoidance());
	this->MainPanelWidget->BindToSpaceShip(InSpaceShipActor);
	InSpaceShipActor->UpdateSpaceShipLevel();
}

void UMainUserWidget::BindToCharacter(ACharacter* InCharacter)
{
}

bool UMainUserWidget::CloseTopWidget_Implementation()
{
	if (this->OpenWidgetStack__.Num() == 0)
	{
		return (true);
	}
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this->OpenWidgetStack__.Last());
	this->OpenWidgetStack__.Pop();
	return (false);
}

void UMainUserWidget::ClearStackWidget_Implementation()
{
	while (!IWidgetStackHostInterface::Execute_CloseTopWidget(this))
	{ }
}

//void UMainUserWidget::BindToCraftingActor(ACraftingActor* InCraftingActor, APlayerCharacter* InPlayerCharacter)
//{
//    InPlayerCharacter->OnToggleInventory.BindUFunction(RecipeListWidget, TEXT("ToggleRecipeListWidget")); // DELETE ME
//
//    this->RecipeListWidget->BindToInventoryCompomnent(IInventoryComponentInterface::Execute_GetInventoryComponent(InPlayerCharacter));
//    this->RecipeListWidget->BindToCraftingComponent(InCraftingActor->GetCraftingComponent());
//    this->RecipeListWidget->OnRecipeSelected.BindUObject(InCraftingActor->GetCraftingComponent(), &UCraftingComponent::HandleRecipeSelected__);
//
//    this->ManufactureWidget->BindToCraftingComponent(InCraftingActor->GetCraftingComponent());
//}
