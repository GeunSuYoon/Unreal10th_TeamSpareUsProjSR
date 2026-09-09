// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/SpaceShipStat/SpaceShipEnergyUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"


void	USpaceShipEnergyUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	// 우주선 델리게이트 아래 함수 연결
}

void USpaceShipEnergyUserWidget::EnergyChange(float InCurrentEnergy, float InMaxEnergy)
{
	this->CurrentEnergyText->SetText(FText::AsNumber(InCurrentEnergy));
	this->MaxEnergyText->SetText(FText::AsNumber(InMaxEnergy));
	this->EnergyProgressBar->SetPercent(InCurrentEnergy / InMaxEnergy);
}
