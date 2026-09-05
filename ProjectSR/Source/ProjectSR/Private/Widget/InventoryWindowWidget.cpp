// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventoryWindowWidget.h"
#include "Widget/ItemManagerWidget.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryComponentInterface.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UInventoryWindowWidget::BindToInventoryComponent(UInventoryComponent* InInventoryComponent)
{
    if (!InInventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryWindowWidget::BindToInventoryComponent()] : InInventoryComponent가 nullptr입니다."));
        return;
    }

    TargetInventory__ = InInventoryComponent;
    ItemManagerWidget->InitializeItemManagerWidget(TargetInventory__.Get());
}

void UInventoryWindowWidget::OpenInventoryWidget()
{
    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryWindowWidget::OpenInventoryWidget()] : TargetInventory가 nullptr입니다."));
        return;
    }

    CapacityText->SetText(FText::FromString("Hello Capacity"));
    WeightText->SetText(FText::FromString("Hello Weight"));
    ItemManagerWidget->RefreshInventoryWidget();

    SetVisibility(ESlateVisibility::Visible);

    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        FInputModeUIOnly InputModeUI;
        InputModeUI.SetWidgetToFocus(TakeWidget());

        PC->SetInputMode(InputModeUI);
        PC->SetShowMouseCursor(true);
    }
}

void UInventoryWindowWidget::CloseInventoryWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);

    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        FInputModeGameOnly InputModeGame;
        PC->SetInputMode(InputModeGame);
        PC->SetShowMouseCursor(false);
    }
}

void UInventoryWindowWidget::ToggleInventoryWidget()
{
    if (IsInventoryOpened())
    {
        CloseInventoryWidget();
    }
    else
    {
        OpenInventoryWidget();
    }
}

void UInventoryWindowWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!Inventory_CloseButton)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryWindowWidget::NativeConstruct()] : Inventory_CloseButton이 nullptr 입니다."));
        return;
    }

    Inventory_CloseButton->OnClicked.AddDynamic(this, &UInventoryWindowWidget::OnInventoryCloseButtonClicked__);

    SetIsFocusable(true);
    CloseInventoryWidget();
}

FReply UInventoryWindowWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::I)
    {
        CloseInventoryWidget();

        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInventoryWindowWidget::OnInventoryCloseButtonClicked__()
{
    CloseInventoryWidget();
}
