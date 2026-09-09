// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/PlayerStatUserWidget.h"
#include "Widget/MainWidget/PlayerStat/PlayerHPBarUserWidget.h"
#include "Widget/MainWidget/PlayerStat/SpaceSuitOxygenUserWidget.h"

void	UPlayerStatUserWidget::BindToPlayer(APlayerCharacter* InPlayer)
{
	this->PlayerHPBar->BindToPlayer(InPlayer);
	this->SpaceSuitOxygen->BindToPlayer(InPlayer);
}
