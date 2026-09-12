// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/PlayMenuUserWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Menu_01Start->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleResumeClicked__);
	Menu_04Exit->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleExitClicked__);
	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Collapsed);
}

void UPlayMenuUserWidget::OpenSelfWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGamePaused(this, true);
	OnWidgetOpen.ExecuteIfBound(this);
}

void UPlayMenuUserWidget::CloseSelfWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
	UGameplayStatics::SetGamePaused(this, false);
	OnWidgetClose.ExecuteIfBound(this);
}

void UPlayMenuUserWidget::HandleResumeClicked__()
{
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}

void UPlayMenuUserWidget::HandleExitClicked__()
{
	UGameplayStatics::SetGamePaused(this, false);
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
