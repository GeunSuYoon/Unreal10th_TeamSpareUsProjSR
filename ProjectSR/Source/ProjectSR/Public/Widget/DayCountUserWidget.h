#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DayCountUserWidget.generated.h"

class ASurvivalLoopActor;
class UTextBlock;

UCLASS()
class PROJECTSR_API UDayCountUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindToSurvivalLoop(ASurvivalLoopActor* InSurvivalLoop);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> DayText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SolarWindDayText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SolarWindDamageText;

private:
	UFUNCTION()
	void HandleDayStatusChanged__(int32 Day, int32 DaysUntilSolarWind, float SolarWindDamage);
};
