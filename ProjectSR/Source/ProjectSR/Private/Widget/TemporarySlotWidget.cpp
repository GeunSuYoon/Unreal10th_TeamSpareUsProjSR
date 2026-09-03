// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TemporarySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UTemporarySlotWidget::InitializeSlot(const FInventorySlot* InSlot)
{
    IconImage->SetBrushFromTexture(InSlot->ItemData->Icon.Get());
    //IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
    CountText->SetText(FText::AsNumber(InSlot->GetCount()));
}

void UTemporarySlotWidget::SetVisual(UTexture2D* InIcon, int32 InCount)
{
    IconImage->SetBrushFromTexture(InIcon);
    CountText->SetText(FText::AsNumber(InCount));
}
