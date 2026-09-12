// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/GameResultUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGameResultUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	GoToMainButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleGoToMainClicked__);
}

void UGameResultUserWidget::SetResult(bool bGameCleared, const FText& InDescription)
{
	ResultText->SetText(bGameCleared ? GameClearTitleText : GameOverTitleText);
	Description->SetText(InDescription);
}

void UGameResultUserWidget::HandleGoToMainClicked__()
{
	OnGoToMainMenu.ExecuteIfBound();
}
