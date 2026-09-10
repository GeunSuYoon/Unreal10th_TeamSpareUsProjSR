// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/PlayerStat/SpaceSuitOxygenUserWidget.h"
#include "Player/PlayerCharacter.h"
#include "Interface/StatComponentInterface.h"
#include "Component/StatComponent.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void	USpaceSuitOxygenUserWidget::BindToPlayer(APlayerCharacter* InPlayer)
{
	// 플레이어의 statcomponent에 oxigen change delegate 만들어서 OnOxigenChange 함수 연결하기
	if (UStatComponent* Stat = IStatComponentInterface::Execute_GetStatComponent(InPlayer))
	{
		Stat->OnOxygenChanged.AddDynamic(this, &USpaceSuitOxygenUserWidget::OxygenChange);
	}
}

void	USpaceSuitOxygenUserWidget::OxygenChange(float InCurrentOxygen, float InMaxOxygen)
{
	this->CurrentOxygenText->SetText(FText::AsNumber(InCurrentOxygen));
	this->MaxOxygenText->SetText(FText::AsNumber(InMaxOxygen));
	this->OxygenProgressBar->SetPercent(InCurrentOxygen / InMaxOxygen);
}
