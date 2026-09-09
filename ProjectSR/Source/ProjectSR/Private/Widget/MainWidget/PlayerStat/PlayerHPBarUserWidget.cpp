// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/PlayerStat/PlayerHPBarUserWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void	UPlayerHPBarUserWidget::BindToPlayer(APlayerCharacter* InPlayer)
{
	// 플레이어 체력 변경 델리게이트 만들여서 HPChange 함수 연결하기
}

void UPlayerHPBarUserWidget::HPChange(float InCurrentHP, float InMaxHP)
{
	this->CurrentHPText->SetText(FText::AsNumber(InCurrentHP));
	this->MaxHPText->SetText(FText::AsNumber(InMaxHP));
	this->HPProgressBar->SetPercent(InCurrentHP / InMaxHP);
}
