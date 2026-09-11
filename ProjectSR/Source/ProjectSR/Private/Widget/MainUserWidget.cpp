// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainUserWidget.h"
#include "Widget/MeteorWarningUserWidget.h"
#include "Widget/MainPanel/MainPanelUserWidget.h"
#include "Widget/InventoryWindowWidget.h"
#include "Widget/Crafting/RecipeListWidget.h"
#include "Widget/Crafting/ManufactureWidget.h"
#include "Widget/MainWidget/InventoryAlarmUserWidget.h"
#include "Widget/MainWidget/PlayerStatUserWidget.h"
#include "Widget/MainWidget/SpaceShipAlarmUserWidget.h"
#include "Widget/MainWidget/SpaceShipStatUserWidget.h"
#include "Widget/ItemManagerWidget.h"

#include "Interface/InventoryComponentInterface.h"
#include "Interface/OpenableWidgetInterface.h"

#include "SpaceShip/SpaceShipActor.h"
#include "Player/PlayerCharacter.h"

#include "InputCoreTypes.h"
#include "GameFramework/PlayerController.h"

void UMainUserWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    SetIsFocusable(true);

    if (MainPanelWidget)
    {
        MainPanelWidget->OnWidgetOpen.BindDynamic(this, &UMainUserWidget::HandleMainPanelOpened__);
        MainPanelWidget->OnWidgetClose.BindDynamic(this, &UMainUserWidget::HandleMainPanelClosed__);
    }
	if (InventoryWidget)
	{
		InventoryWidget->OnWidgetOpen.BindDynamic(this, &UMainUserWidget::HandleMainPanelOpened__);
		InventoryWidget->OnWidgetClose.BindDynamic(this, &UMainUserWidget::HandleMainPanelClosed__);
	}
}

void UMainUserWidget::HandleMainPanelOpened__(UUserWidget* InWidget)
{
    RegisterOpenWidget__(InWidget);
}

void UMainUserWidget::HandleMainPanelClosed__(UUserWidget* InWidget)
{
    UnregisterOpenWidget__(InWidget);
}

void UMainUserWidget::BindWidget__()
{
	if (!this->bIsSpaceShipBind || !this->bIsPlayerBind)
	{
		return ;
	}
	this->MainPanelWidget->GetWarehouseWidget()->OnItemManagerOpen.BindUObject(this->InventoryWidget, &UInventoryWindowWidget::OpenWidget);
	this->MainPanelWidget->GetWarehouseWidget()->OnItemManagerClose.BindUObject(this->InventoryWidget, &UInventoryWindowWidget::CloseWidget);
	this->MainPanelWidget->GeCraftingRecipeListWidget()->OnCraftingOpen.BindUObject(this->CraftingManufacture, &UManufactureWidget::OpenWidget);
	this->MainPanelWidget->GeCraftingRecipeListWidget()->OnCraftingClose.BindUObject(this->CraftingManufacture, &UManufactureWidget::CloseWidget);
}

void UMainUserWidget::RegisterOpenWidget__(UUserWidget* Widget)
{
    if (!IsValid(Widget) || !Widget->GetClass()->ImplementsInterface(UOpenableWidgetInterface::StaticClass()))
    {
        return;
    }

    if (!OpenWidgetStack__.Contains(Widget))
    {
        OpenWidgetStack__.Add(Widget);
    }
    UpdateInputMode__();
}

void UMainUserWidget::UnregisterOpenWidget__(UUserWidget* Widget)
{
    // Closing through ESC also triggers the widget's close delegate.
    // Removing by identity makes the second notification harmless.
    if (OpenWidgetStack__.Remove(Widget) > 0)
    {
        UpdateInputMode__();
    }
}

void UMainUserWidget::UpdateInputMode__()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    if (!OpenWidgetStack__.IsEmpty())
    {
        UUserWidget* TopWidget = OpenWidgetStack__.Last();
        UUserWidget* FocusWidget = IsValid(TopWidget) && TopWidget->IsFocusable() ? TopWidget : this;
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(true);
    }
    else
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->SetShowMouseCursor(false);
    }
}

FReply UMainUserWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Escape && !OpenWidgetStack__.IsEmpty())
    {
        // Holding ESC must not close several stacked windows.
        if (!InKeyEvent.IsRepeat())
        {
            IWidgetStackHostInterface::Execute_CloseTopWidget(this);
        }
        return FReply::Handled();
    }
    if (!OpenWidgetStack__.IsEmpty() && MainPanelWidget
        && OpenWidgetStack__.Last().Get() == MainPanelWidget.Get()
        && MainPanelWidget->HandleMeteorMoveKey(InKeyEvent))
    {
        return FReply::Handled();
    }
    return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

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
    InPlayerCharacter->OnToggleInventory.BindUFunction(InventoryWidget, TEXT("ToggleInventoryWidget"));
    this->InventoryWidget->BindToInventoryComponent(IInventoryComponentInterface::Execute_GetInventoryComponent(InPlayerCharacter));
	this->InventoryAlarm->BindToInventory(IInventoryComponentInterface::Execute_GetInventoryComponent(InPlayerCharacter));
	this->PlayerStatWidget->BindToPlayer(InPlayerCharacter);
	InPlayerCharacter->InitBroadCast();
	this->bIsPlayerBind = true;
	this->BindWidget__();
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
	this->MainPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
	this->MainPanelWidget->BindToSpaceShip(InSpaceShipActor);
	this->SpaceShipAlarm->BindToSpaceShip(InSpaceShipActor);
	this->SpaceShipStat->BindToSpaceShip(InSpaceShipActor);
	this->CraftingManufacture->BindToCraftingComponent(InSpaceShipActor->GetCraftingComponent());
	InSpaceShipActor->InitBroadCast();
	this->bIsSpaceShipBind = true;
	this->BindWidget__();
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
	UUserWidget* ClosingWidget = this->OpenWidgetStack__.Last();
	if (IsValid(ClosingWidget))
	{
		IOpenableWidgetInterface::Execute_CloseSelfWidget(ClosingWidget);
	}
	UnregisterOpenWidget__(ClosingWidget);
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
