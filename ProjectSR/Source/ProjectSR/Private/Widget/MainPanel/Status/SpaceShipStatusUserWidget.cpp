// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Status/SpaceShipStatusUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/TextBlock.h"

void	USpaceShipStatusUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	InSpaceShip->OnSpaceShipLevelChange.BindUFunction(this, TEXT("UpdateStat__"));
	InSpaceShip->OnDurabilityChange.AddDynamic(this, &USpaceShipStatusUserWidget::UpdateDurability__);
	InSpaceShip->OnEnergyChange.AddDynamic(this, &USpaceShipStatusUserWidget::UpdateEnergy__);
}

bool USpaceShipStatusUserWidget::bIsLevelup(int32 InLevel)
{
	if (InLevel < this->LevelValue__)
	{
		return (true);
	}
	return (false);
}

void USpaceShipStatusUserWidget::UpdateStat__(const FSpaceShipStat& InSpaceShipStat)
{
	this->LevelValue__ = InSpaceShipStat.Level;
	this->Level->SetText(FText::AsNumber(InSpaceShipStat.Level));
	this->CurrentDurability->SetText(FText::AsNumber(InSpaceShipStat.MaxDurability));
	this->MaxDurability->SetText(FText::AsNumber(InSpaceShipStat.MaxDurability));
	this->CurrentEnergy->SetText(FText::AsNumber(InSpaceShipStat.MaxEnergy));
	this->MaxEnergy->SetText(FText::AsNumber(InSpaceShipStat.MaxEnergy));
	this->OperationalEnergy->SetText(FText::AsNumber(InSpaceShipStat.OperationalEnergy));
	this->CurrentSpeed->SetText(FText::AsNumber(InSpaceShipStat.MoveSpeed));
	//this->CurrentCapacity->SetText(FText::AsNumber(InSpaceShipStat.MaxCapacity));
	this->MaxCapacity->SetText(FText::AsNumber(InSpaceShipStat.MaxCapacity));
}

void USpaceShipStatusUserWidget::UpdateDurability__(float InCurrentDurability, float InMaxDurability)
{
	this->CurrentDurability->SetText(FText::AsNumber(InCurrentDurability));
	this->MaxDurability->SetText(FText::AsNumber(InMaxDurability));
}

void USpaceShipStatusUserWidget::UpdateEnergy__(float InCurrentEnergy, float InMaxEnergy)
{
	this->CurrentEnergy->SetText(FText::AsNumber(InCurrentEnergy));
	this->MaxEnergy->SetText(FText::AsNumber(InMaxEnergy));
}

void USpaceShipStatusUserWidget::UpdateCurrentDurability__(float InDurability)
{
	this->CurrentDurability->SetText(FText::AsNumber(InDurability));
}

void USpaceShipStatusUserWidget::UpdateCurrentEnergy__(float InEnergy)
{
	this->CurrentEnergy->SetText(FText::AsNumber(InEnergy));
}

void USpaceShipStatusUserWidget::UpdateCurrentCapacity__(float InCapacity)
{
	this->CurrentCapacity->SetText(FText::AsNumber(InCapacity));
}
