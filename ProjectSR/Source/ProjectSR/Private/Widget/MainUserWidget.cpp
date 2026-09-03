// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainUserWidget.h"
#include "Widget/MeteorWarningUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"

void UMainUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShipActor)
{
	if (!InSpaceShipActor)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainUserWidget::BindToSpaceShip] InSpaceShipActor가 nullptr입니다.")
		);
		return;
	}
	this->MeteoWarningWidget->SetVisibility(ESlateVisibility::Collapsed);
	this->MeteoWarningWidget->BindToMeteorAvoidanceComponent(InSpaceShipActor->GetMeteorAvoidance());
}
