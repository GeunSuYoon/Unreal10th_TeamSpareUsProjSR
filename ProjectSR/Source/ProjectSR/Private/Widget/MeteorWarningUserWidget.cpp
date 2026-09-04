// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MeteorWarningUserWidget.h"
#include "Components/TextBlock.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"

void UMeteorWarningUserWidget::BindToMeteorAvoidanceComponent(UMeteorAvoidanceComponent* InMeteorAvoidanceComponent)
{
	if (!InMeteorAvoidanceComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMeteorWarningUserWidget::BindToMeteorAvoidanceComponent] InMeteorAvoidanceComponent가 nullptr입니다.")
		);
		return ;
	}
	InMeteorAvoidanceComponent->OnMeteorDetect.AddDynamic(this, &UMeteorWarningUserWidget::MeteorWarningStart__);
	InMeteorAvoidanceComponent->OnMeteorTimer.BindUFunction(this, TEXT("MeteorWarningTimer__"));
	InMeteorAvoidanceComponent->OnMeteorClear.AddDynamic(this, &UMeteorWarningUserWidget::MeteorWarningEnd__);
}

void UMeteorWarningUserWidget::MeteorWarningStart__(const FMeteor& InMeteor)
{
	PlayAnimation(this->MeteoWarningStartAnim);
	this->MeteoSize->SetText(FText::AsNumber(InMeteor.MeteorSize));
	this->MeteoDamage->SetText(FText::AsNumber(InMeteor.MeteorDamage));
	this->RemainedSecond->SetText(FText::AsNumber(InMeteor.MeteorRemainTime));
	SetVisibility(ESlateVisibility::Visible);
}

void UMeteorWarningUserWidget::MeteorWarningTimer__(const FMeteor& InMeteor)
{
	this->RemainedSecond->SetText(FText::AsNumber(InMeteor.MeteorRemainTime));
}

void UMeteorWarningUserWidget::MeteorWarningEnd__()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
