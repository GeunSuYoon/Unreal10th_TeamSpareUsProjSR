// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Status/MachineArmStatusUserWidget.h"
#include "SpaceShip/MachineArmComponent.h"

#include "Components/TextBlock.h"

void UMachineArmStatusUserWidget::BindToMachineArmComponent(UMachineArmComponent* InMachineArm)
{
	InMachineArm->OnMachineArmLevelChange.BindUFunction(this, TEXT("UpdateStat__"));
}

void UMachineArmStatusUserWidget::UpdateStat__(const FMachineArmStat& InMachineArmStat)
{
	this->LevelValue__ = InMachineArmStat.Level;
	this->Level->SetText(FText::AsNumber(InMachineArmStat.Level));
	this->ItemCollectTime->SetText(FText::AsNumber(InMachineArmStat.ItemCollectTime));
	this->ItemCollectWeight->SetText(FText::AsNumber(InMachineArmStat.ItemCollectWeight));
	this->OperationalEnergy->SetText(FText::AsNumber(InMachineArmStat.OperationalEnergy));
}
