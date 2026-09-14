// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/SpaceShipAlarmUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void	USpaceShipAlarmUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	this->BoundSpaceShip__ = InSpaceShip;
	InitVisibility();
	InSpaceShip->OnDurabilityChange.AddDynamic(this, &USpaceShipAlarmUserWidget::SpaceShipDurabilityChange);
	InSpaceShip->OnEnergyChange.AddDynamic(this, &USpaceShipAlarmUserWidget::SpaceShipEnergyChange);
	SpaceShipEnergyChange(InSpaceShip->GetCurrentEnergy(), InSpaceShip->GetMaxEnergy());
}

void USpaceShipAlarmUserWidget::SpaceShipEnergyChange(float InCurrentEnergy, float InMaxEnergy)
{
	const float RequiredOperationalEnergy = this->BoundSpaceShip__->GetStat().OperationalEnergy;
	const bool bWasVisible = EnergyAlarmHorizontalBox->IsVisible();
	this->CurrentEnergy->SetText(FText::AsNumber(InCurrentEnergy));
	this->OperationalEnergy->SetText(FText::AsNumber(RequiredOperationalEnergy));
	if (InCurrentEnergy < RequiredOperationalEnergy)
	{
		this->EnergyAlarmHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		this->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->EnergyAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	this->CheckChildVisibility();
	if (!bWasVisible && EnergyAlarmHorizontalBox->IsVisible() && EnergyAlarmSFX)
	{
		UGameplayStatics::PlaySound2D(this, EnergyAlarmSFX);
	}
}

void USpaceShipAlarmUserWidget::SpaceShipDurabilityChange(float InCurrentDurability, float InMaxDurability)
{
	this->CurrentDurability->SetText(FText::AsNumber(InCurrentDurability));
	this->CurrentDurability__ = InCurrentDurability;
	this->CheckDurability();
	this->CheckChildVisibility();
}

void USpaceShipAlarmUserWidget::RequiredDurabilityChange(float InRequiredDurability)
{
	this->RequiredDurability->SetText(FText::AsNumber(InRequiredDurability));
	this->RequiredDurability__ = InRequiredDurability;
	this->CheckDurability();
	this->CheckChildVisibility();
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

void USpaceShipAlarmUserWidget::CheckDurability()
{
	const bool bWasVisible = DurabilityAlarmHorizontalBox->IsVisible();
	if (this->CurrentDurability__ <= this->RequiredDurability__)
	{
		this->DurabilityAlarmHorizontalBox->SetVisibility(ESlateVisibility::Visible);
		this->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		this->DurabilityAlarmHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (!bWasVisible && DurabilityAlarmHorizontalBox->IsVisible() && DurabilityAlarmSFX)
	{
		UGameplayStatics::PlaySound2D(this, DurabilityAlarmSFX);
	}
}
