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
    TargetInventory__->OnSlotChanged.AddDynamic(this, &UInventoryWindowWidget::RefreshCapacityText__);
    ItemManagerWidget->BindToInventoryComponent(TargetInventory__.Get());
    ItemManagerWidget->InitializeInventoryWidget();
}

void UInventoryWindowWidget::OpenWidget()
{
	IOpenableWidgetInterface::Execute_OpenSelfWidget(this);
}

void UInventoryWindowWidget::CloseWidget()
{
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}

void UInventoryWindowWidget::OpenSelfWidget_Implementation()
{
	if (!TargetInventory__.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[UInventoryWindowWidget::OpenInventoryWidget()] : TargetInventory가 nullptr입니다."));
		return;
	}

	RefreshCapacityText__(0);
	ItemManagerWidget->RefreshInventoryWidget();

	SetVisibility(ESlateVisibility::Visible);
	OnWidgetOpen.ExecuteIfBound(this);
}

void UInventoryWindowWidget::CloseSelfWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
	OnWidgetClose.ExecuteIfBound(this);
}

void UInventoryWindowWidget::ToggleInventoryWidget()
{
    if (GetVisibility() == ESlateVisibility::Visible)
    {
		IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
    }
    else
    {
		IOpenableWidgetInterface::Execute_OpenSelfWidget(this);
    }
}

void UInventoryWindowWidget::RefreshCapacityText__(int32 InSlotIndex)
{
    CurrentCapacityText->SetText(FText::AsNumber(TargetInventory__->GetUsingSlotCount()));
    MaxCapacityText->SetText(FText::AsNumber(TargetInventory__->GetSize()));
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
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}

FReply UInventoryWindowWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::I)
    {
		IOpenableWidgetInterface::Execute_CloseSelfWidget(this);

        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UInventoryWindowWidget::OnInventoryCloseButtonClicked__()
{
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}
