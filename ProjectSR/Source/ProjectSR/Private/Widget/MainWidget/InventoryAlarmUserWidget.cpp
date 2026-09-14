// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/InventoryAlarmUserWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UInventoryAlarmUserWidget::BindToInventory(UInventoryComponent* InInventory)
{
	// 인벤토리 컴포넌트의 델리게이트에 아래 함수 바인드하기
	InInventory->OnSlotSize.AddDynamic(this, &UInventoryAlarmUserWidget::InventorySlotChange);
	this->InitVisibility();
}

void UInventoryAlarmUserWidget::InventorySlotChange(int32 InCurrentSize, int32 InMaxSize)
{
	const bool bWasVisible = InventorySlotHorizontalBox->IsVisible();
	this->CurrentCount->SetText(FText::AsNumber(InCurrentSize));
	this->MaxCount->SetText(FText::AsNumber(InMaxSize));
	if (AlramRate <= static_cast<float>(InCurrentSize) / static_cast<float>(InMaxSize))
	{
		this->InventorySlotHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		this->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->InventorySlotHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		if (this->InventoryWeightHorizontalBox->GetVisibility() == ESlateVisibility::Collapsed)
		{
			this->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (!bWasVisible && InventorySlotHorizontalBox->IsVisible() && InventoryAlarmSFX)
	{
		UGameplayStatics::PlaySound2D(this, InventoryAlarmSFX);
	}
}

void UInventoryAlarmUserWidget::InitVisibility()
{
	this->InventorySlotHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	this->InventoryWeightHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	this->SetVisibility(ESlateVisibility::Collapsed);
}
