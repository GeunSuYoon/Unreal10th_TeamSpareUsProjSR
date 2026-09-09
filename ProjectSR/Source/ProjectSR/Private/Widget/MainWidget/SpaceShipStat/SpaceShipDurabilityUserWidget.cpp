// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/SpaceShipStat/SpaceShipDurabilityUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void	USpaceShipDurabilityUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	//SpaceShipActor에 델리게이트 생성 후 아래 함수 바인드
}

void USpaceShipDurabilityUserWidget::DurabilityChange(float InCurrentDurability, float InMaxDurability)
{
	this->CurrentDurabilityText->SetText(FText::AsNumber(InCurrentDurability));
	this->MaxDurabilityText->SetText(FText::AsNumber(InMaxDurability));
	this->DurabilityProgressBar->SetPercent(InCurrentDurability / InMaxDurability);
}
