// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventorySlotWidget.h"
#include "Widget/TemporarySlotWidget.h"
#include "Component/InventoryComponent.h"
#include "CommonHeader/InventoryDragDropOperation.h"

#include "Components/Image.h"
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
        Item_Grid_Icon->SetBrushFromTexture(nullptr);
        Item_Grid_Icon->SetBrushTintColor(FLinearColor::Transparent);
        Item_Grid_Count->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        Item_Grid_Icon->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
        Item_Grid_Icon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        Item_Grid_Count->SetText(FText::AsNumber(TargetSlot->GetCount()));
        Item_Grid_Count->SetVisibility(ESlateVisibility::Hidden);
    }
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        if (FInventorySlot* InventorySlot = TargetInventory__->GetSlot(Index__))
        {
            if (!InventorySlot->IsEmpty())
            {
                return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
            }
        }
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    OnSlotClicked.ExecuteIfBound(Index__);

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return;
    }

    FInventorySlot* SourceSlot = TargetInventory__->GetSlot(Index__);

    if (!SourceSlot || SourceSlot->IsEmpty())
    {
        return;
    }

    OnDragStarted.ExecuteIfBound();

    UInventoryDragDropOperation* DragOp = NewObject<UInventoryDragDropOperation>();
    DragOp->SourceInventory = TargetInventory__;
    DragOp->SourceIndex = Index__;

    UTemporarySlotWidget* TempSlotWidget = CreateWidget<UTemporarySlotWidget>(
        this,
        TargetInventory__->GetTemporarySlotWidgetClass()
    );
    TempSlotWidget->InitializeSlot(TargetInventory__->GetSlot(Index__));
    TempSlotWidget->SetVisual(SourceSlot->ItemData->Icon.Get(), SourceSlot->GetCount());

    DragOp->DefaultDragVisual = TempSlotWidget; // 얘 전용 레이어가 따로 생겼다가 드래그가 끝나면 사라짐. 따라서 AddToViewport 안해줘도 됨

    OutOperation = DragOp; // NativeOnDrop과 NariveOnDragCancelled를 발동시키기 위해 필수

    FInventoryCommandResult Result;
    TargetInventory__->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(TargetInventory__.Get(), Index__, TargetInventory__->GetTempSlotIndex()),
        Result);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // false 리턴하면 OnDragCancelled 실행해버림
    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventorySlotWidget::NativeOnDrop()] : TargetInventory가 nullptr입니다."));
        return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    }

    UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);

    if (DragOp->SourceInventory->GetTempSlot()->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventorySlotWidget::NativeOnDrop()] : DragOp->SourceInventory의 TempSlot이 비어있습니다."));
        return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    }

    FInventoryCommandResult Result;

    TargetInventory__->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(DragOp->SourceInventory.Get(), DragOp->SourceInventory->GetTempSlotIndex(), Index__),
        Result);

    TargetInventory__->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(DragOp->SourceInventory.Get(), DragOp->SourceInventory->GetTempSlotIndex(), DragOp->SourceIndex),
        Result);

    return true;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlotWidget::OnSlotButtonClicked__()
{
    OnSlotClicked.ExecuteIfBound(Index__);
}
