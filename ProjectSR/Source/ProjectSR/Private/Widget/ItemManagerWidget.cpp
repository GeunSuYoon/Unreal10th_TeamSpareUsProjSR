// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ItemManagerWidget.h"
#include "Widget/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryComponentInterface.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameFramework/PlayerController.h"

void UItemManagerWidget::BindToInventoryComponent(UInventoryComponent* InInventoryComponent)
{
    if (!InInventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::BindToInventoryComponent()] : InInventoryComponent가 nullptr 입니다."));
        return;
    }

    TargetInventory__ = InInventoryComponent;
    TargetInventory__->OnSlotChanged.AddDynamic(this, &UItemManagerWidget::RefreshSlotWidget__);
}

void UItemManagerWidget::InitializeInventoryWidget()
{
    if (!Item_Use)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeInventoryWidget()] : Item_Use 버튼이 nullptr 입니다."));
        return;
    }

    Item_Use->OnClicked.AddDynamic(this, &UItemManagerWidget::OnItemUseButtonClicked__);

    if (!Item_Drop)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeInventoryWidget()] : Item_Drop 버튼이 nullptr 입니다."));
        return;
    }

    Item_Drop->OnClicked.AddDynamic(this, &UItemManagerWidget::OnItemDropButtonClicked__);

    RefreshInventoryWidget();
}

void UItemManagerWidget::RefreshInventoryWidget()
{
    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshInventoryWidget()] : TargetInventory가 nullptr 입니다."));
        return;
    }

    if (!ItemGridPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshInventoryWidget()] : ItemGridPanel이 nullptr 입니다."));
        return;
    }

    SelectedSlotIndex__ = InvalidIndex;

    RefreshItemDetailPanel__();
    ClearInventoryWidget();

    Capacity__ = TargetInventory__->GetSize();
    SlotWidgets__.Empty(Capacity__);

    const int32 ColumnSize = 4;
    for (int i = 0; i < Capacity__; i++)
    {
        UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), InventorySlotWidgetClass);
        if (SlotWidget)
        {
            UUniformGridSlot* GridSlot = ItemGridPanel->AddChildToUniformGrid(SlotWidget, i / ColumnSize, i % ColumnSize);
            if (GridSlot)
            {
                GridSlot->SetHorizontalAlignment(HAlign_Fill);
                GridSlot->SetVerticalAlignment(VAlign_Fill);
            }

            SlotWidget->BindToInventoryComponent(TargetInventory__.Get());
            SlotWidget->InitializeSlot(i);

            SlotWidget->OnSlotClicked.BindWeakLambda(
                this,
                [this](int InIndex) {
                    if (TargetInventory__.IsValid())
                    {
                        SelectedSlotIndex__ = InIndex;
                        RefreshItemDetailPanel__();
                    }
                }
            );

            SlotWidget->OnDragStarted.BindWeakLambda(
                this,
                [this]() {
                    if (TargetInventory__.IsValid())
                    {
                        SelectedSlotIndex__ = InvalidIndex;
                        RefreshItemDetailPanel__();
                    }
                }
            );

            SlotWidgets__.Add(SlotWidget);
        }
    }
}

void UItemManagerWidget::ClearInventoryWidget()
{
    SlotWidgets__.Empty();
    Capacity__ = 0;

    ItemGridPanel->ClearChildren();
}

void UItemManagerWidget::RefreshSlotWidget__(int32 InSlotIndex) const
{
    // 드래그 전용 슬롯은 갱신 무시
    if (InSlotIndex == TargetInventory__->GetTempSlotIndex())
    {
        return;
    }

    if (!IsValidIndex__(InSlotIndex) || !SlotWidgets__[InSlotIndex])
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshSlotWidget__()] : InSlotIndex가 유효하지 않습니다."));
        return;
    }

    SlotWidgets__[InSlotIndex]->RefreshSlot();
    RefreshItemDetailPanel__();
}

void UItemManagerWidget::RefreshItemDetailPanel__() const
{
    ItemInfoPanel->SetVisibility(ESlateVisibility::Hidden);

    // 아이템 정보를 비우도록 의도된 InvalidIndex 설정이므로 바로 리턴
    if (SelectedSlotIndex__ == InvalidIndex)
    {
        return;
    }

    if (!IsValidIndex__(SelectedSlotIndex__) || !SlotWidgets__[SelectedSlotIndex__])
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshItemDetailPanel__()] : SelectedSlotIndex가 유효하지 않습니다."));
        return;
    }

    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshItemDetailPanel__()] : TargetInventory가 nullptr입니다."));
        return;
    }

    const FInventorySlot* TargetSlot = TargetInventory__->GetSlot(SelectedSlotIndex__);

    if (!TargetSlot->IsEmpty())
    {
        Iteminfo_Image->SetBrushFromTexture(TargetSlot->ItemData->Icon.LoadSynchronous());
        Iteminfo_Image->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        Iteminfo_Name->SetText(FText::FromName(TargetSlot->ItemData->ItemId));

        FText CapacityText = FText::Format(
            NSLOCTEXT("Inventory", "SlotCountFormat", "수량 : {0} / {1} 개"),
            FText::AsNumber(TargetSlot->GetCount()),
            FText::AsNumber(TargetSlot->ItemData->MaxStackCount)
        );
        Iteminfo_Count->SetText(CapacityText);
        Iteminfo_Description->SetText(TargetSlot->ItemData->Description);

        ItemInfoPanel->SetVisibility(ESlateVisibility::Visible);
    }
}

void UItemManagerWidget::OnItemUseButtonClicked__()
{
    if (!IsValidIndex__(SelectedSlotIndex__))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::OnItemUseButtonClicked__()] : SelectedSlotIndex가 유효하지 않습니다."));
        return;
    }

    FInventoryCommandResult Result;
    TargetInventory__->ExecuteCommand(
        FInventoryCommand::MakeUseCommand(SelectedSlotIndex__),
        Result
    );
}

void UItemManagerWidget::OnItemDropButtonClicked__()
{
    if (!IsValidIndex__(SelectedSlotIndex__))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::OnItemDropButtonClicked__()] : SelectedSlotIndex가 유효하지 않습니다."));
        return;
    }

    FInventoryCommandResult Result;
    TargetInventory__->ExecuteCommand(
        FInventoryCommand::MakeDropCommand(SelectedSlotIndex__, GetOwningPlayerPawn()->GetActorLocation()),
        Result
    );
}
