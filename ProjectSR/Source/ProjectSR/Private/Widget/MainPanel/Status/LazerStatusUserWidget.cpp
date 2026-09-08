// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Status/LazerStatusUserWidget.h"
#include "SpaceShip/LazerComponent.h"

#include "Components/TextBlock.h"

void ULazerStatusUserWidget::BindToLazerComponent(ULazerComponent* InLazer)
{
	InLazer->OnLazerLevelChange.BindUFunction(this, TEXT("UpdateStat__"));
}

void ULazerStatusUserWidget::UpdateStat__(const FLazerStat& InLazerStat)
{
	this->LevelValue__ = InLazerStat.Level;
	this->Level->SetText(FText::AsNumber(InLazerStat.Level));
	this->Damage->SetText(FText::AsNumber(InLazerStat.Damage));
	this->ReactiveEnergy->SetText(FText::AsNumber(InLazerStat.ReactiveEnergy));
	this->OperationalEnergy->SetText(FText::AsNumber(InLazerStat.OperationalEnergy));
}
