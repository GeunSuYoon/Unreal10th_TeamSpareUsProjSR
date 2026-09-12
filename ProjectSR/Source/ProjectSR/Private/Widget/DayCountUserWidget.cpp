#include "Widget/DayCountUserWidget.h"

#include "Components/TextBlock.h"
#include "Framework/SurvivalLoopActor.h"

void UDayCountUserWidget::BindToSurvivalLoop(ASurvivalLoopActor* InSurvivalLoop)
{
	InSurvivalLoop->OnDayStatusChanged.AddUniqueDynamic(this, &ThisClass::HandleDayStatusChanged__);
}

void UDayCountUserWidget::HandleDayStatusChanged__(int32 Day, int32 DaysUntilSolarWind, float SolarWindDamage)
{
	DayText->SetText(FText::AsNumber(Day));
	SolarWindDayText->SetText(FText::AsNumber(DaysUntilSolarWind));
	SolarWindDamageText->SetText(FText::AsNumber(SolarWindDamage));
}
