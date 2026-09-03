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
#include "GameFramework/PlayerController.h"

void UItemManagerWidget::InitializeItemManagerWidget(UInventoryComponent* InInventoryComponent)
{
    ClearInventoryWidget();

    if (!InInventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeItemManagerWidget()] : InInventoryComponent가 nullptr 입니다."));
        return;
    }

    TargetInventory__ = InInventoryComponent;
    TargetInventory__->OnSlotChanged.BindUObject(this, &UItemManagerWidget::RefreshSlotWidget_);

    if (!ItemGridPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeItemManagerWidget()] : ItemGridPanel이 nullptr 입니다."));
        return;
    }

    int32 ChildCount = ItemGridPanel->GetChildrenCount();
    int32 InventorySize = TargetInventory__->GetSize();

    SlotWidgets__.Empty(SlotSize__);
    SlotSize__ = FMath::Min(ChildCount, InventorySize);

    for (int i = 0; i < SlotSize__; i++)
    {
        if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(ItemGridPanel->GetChildAt(i)))
        {
            SlotWidget->InitializeSlot(TargetInventory__.Get(), i);

            SlotWidget->OnSlotClicked.BindWeakLambda(
                this,
                [this](int InIndex) {
                    if (TargetInventory__.IsValid())
                    {
                        SelectedSlotIndex__ = InIndex;
                        RefreshItemDetailPanel_();
                    }
                }
            );

            SlotWidget->OnDragStarted.BindWeakLambda(
                this,
                [this]() {
                    if (TargetInventory__.IsValid())
                    {
                        SelectedSlotIndex__ = InvalidIndex;
                        RefreshItemDetailPanel_();
                    }
                }
            );

            SlotWidgets__.Add(SlotWidget);
        }
    }

    if (!Item_Use)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeItemManagerWidget()] : Item_Use 버튼이 nullptr 입니다."));
        return;
    }

    Item_Use->OnClicked.AddDynamic(this, &UItemManagerWidget::OnItemUseButtonClicked__);

    if (!Item_Drop)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::InitializeItemManagerWidget()] : Item_Drop 버튼이 nullptr 입니다."));
        return;
    }

    Item_Drop->OnClicked.AddDynamic(this, &UItemManagerWidget::OnItemDropButtonClicked__);

    RefreshInventoryWidget();
}

void UItemManagerWidget::ClearInventoryWidget()
{
    if (TargetInventory__.IsValid())
    {
        TargetInventory__->OnSlotChanged.Unbind();
        //TargetInventory__->OnMoneyChanged.RemoveAll(this);
        TargetInventory__ = nullptr;
    }

    SlotWidgets__.Empty();
    SlotSize__ = 0;
}

void UItemManagerWidget::RefreshInventoryWidget()
{
    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshInventoryWidget_()] : TargetInventory가 nullptr 입니다."));
        return;
    }

    SelectedSlotIndex__ = InvalidIndex;
    RefreshItemDetailPanel_();

    for (const UInventorySlotWidget* SlotWidget : SlotWidgets__)
    {
        if (SlotWidget)
        {
            SlotWidget->RefreshSlot();
        }
    }
}

void UItemManagerWidget::RefreshSlotWidget_(int32 InSlotIndex) const
{
    if (!IsValidIndex__(InSlotIndex) || !SlotWidgets__[InSlotIndex])
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshSlotWidget_()] : InSlotIndex가 유효하지 않습니다."));
        return;
    }

    SlotWidgets__[InSlotIndex]->RefreshSlot();
    RefreshItemDetailPanel_();
}

void UItemManagerWidget::RefreshItemDetailPanel_() const
{
    ItemInfoPanel->SetVisibility(ESlateVisibility::Hidden);

    // 아이템 정보를 비우도록 의도된 InvalidIndex 설정이므로 바로 리턴
    if (SelectedSlotIndex__ == InvalidIndex)
    {
        return;
    }

    if (!IsValidIndex__(SelectedSlotIndex__) || !SlotWidgets__[SelectedSlotIndex__])
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshItemDetailPanel_()] : SelectedSlotIndex가 유효하지 않습니다."));
        return;
    }

    if (!TargetInventory__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemManagerWidget::RefreshItemDetailPanel_()] : TargetInventory가 nullptr입니다."));
        return;
    }

    const FInventorySlot* TargetSlot = TargetInventory__->GetSlot(SelectedSlotIndex__);

    if (!TargetSlot->IsEmpty())
    {
        Iteminfo_Image->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
        Iteminfo_Image->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        Iteminfo_Name->SetText(FText::FromName(TargetSlot->ItemData->ItemId));
        Iteminfo_Weight->SetText(FText::AsNumber(TargetSlot->ItemData->Weight));
        Iteminfo_Count->SetText(FText::AsNumber(TargetSlot->GetCount()));
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
