// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h" 

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex)
{
    if (!InInventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryWidget::InitializeSlot()] : InInventoryComponent가 nullptr 입니다."));
        return;
    }

    TargetInventory__ = InInventoryComponent;
    Index__ = InSlotIndex;

    Item_GridButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnSlotButtonClicked__);

    RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
    if (!TargetInventory__.IsValid())
    {
        return;
    }

    const FInventorySlot* TargetSlot = TargetInventory__->GetSlot(Index__);

    if (!TargetSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 유효하지 않습니다."), Index__);
        return;
    }

    if (TargetSlot->IsEmpty())
    {
        FButtonStyle NewStyle = Item_GridButton->GetStyle();

        FSlateBrush NewBrush;
        NewBrush.SetResourceObject(nullptr);

        NewStyle.SetNormal(NewBrush);

        Item_GridButton->SetStyle(NewStyle);
        Item_Grid_Count->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        FButtonStyle NewStyle = Item_GridButton->GetStyle();

        FSlateBrush NewBrush;
        NewBrush.SetResourceObject(TargetSlot->ItemData->Icon.Get());

        NewStyle.SetNormal(NewBrush);

        Item_GridButton->SetStyle(NewStyle);
        Item_Grid_Count->SetText(FText::AsNumber(TargetSlot->GetCount()));
        Item_Grid_Count->SetVisibility(ESlateVisibility::Hidden);
    }
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    return false;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlotWidget::OnSlotButtonClicked__()
{
    OnSlotClicked.ExecuteIfBound(Index__);
}
