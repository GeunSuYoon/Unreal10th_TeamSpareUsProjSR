// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/SpaceShipAlarmUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void	USpaceShipAlarmUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	// 바인드해야해용
}

void USpaceShipAlarmUserWidget::SpaceShipEnergyChange(float InCurrentEnergy, float InOperationalEnergy)
{
	if (InCurrentEnergy < InOperationalEnergy)
	{
		this->EnergyAlarmHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		this->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->EnergyAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	CheckChildVisibility();
}

void USpaceShipAlarmUserWidget::SpaceShipDurabilityChange(float InCurrentDurability, float InRequiredDurability)
{
	if (InCurrentDurability <= InRequiredDurability)
	{
		this->DurabilityAlarmHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		this->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->DurabilityAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	CheckChildVisibility();
}

void	USpaceShipAlarmUserWidget::InitVisibility()
{
	this->EnergyAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	this->DurabilityAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void USpaceShipAlarmUserWidget::CheckChildVisibility()
{
	if (this->EnergyAlarmHorizontalBox->GetVisibility() == ESlateVisibility::Collapsed &&
		this->DurabilityAlarmHorizontalBox->GetVisibility() == ESlateVisibility::Collapsed)
	{
		this->SetVisibility(ESlateVisibility::Collapsed);
	}
}
