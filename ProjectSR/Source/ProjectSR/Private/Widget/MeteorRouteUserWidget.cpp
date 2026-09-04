// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MeteorRouteUserWidget.h"
//#include "MeteorRouteUserWidget.h"

void UMeteorRouteUserWidget::SetVisibleToggle(bool bInIsVisible)
{
	if (bInIsVisible)
	{
		this->SetVisibility(ESlateVisibility::Visible);
		if (this->ArrowFlowAnim)
		{
			PlayAnimation(
				this->ArrowFlowAnim,
				0.0f,
				0,
				EUMGSequencePlayMode::Forward,
				1.0f
			);
		}
	}
	else
	{
		this->SetVisibility(ESlateVisibility::Collapsed);
		if (this->ArrowFlowAnim)
		{
			StopAnimation(this->ArrowFlowAnim);
		}
	}
}

void UMeteorRouteUserWidget::NativeConstruct()
{
	this->SetVisibleToggle(false);
}
