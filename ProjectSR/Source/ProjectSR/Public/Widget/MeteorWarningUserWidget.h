// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Blueprint/UserWidget.h"
#include "MeteorWarningUserWidget.generated.h"

class UTextBlock;
class UMeteorAvoidanceComponent;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UMeteorWarningUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void	BindToMeteorAvoidanceComponent(UMeteorAvoidanceComponent* InMeteorAvoidanceComponent);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	MeteoSize;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	MeteoDamage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	RemainedSecond;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation>	MeteoWarningStartAnim;

private:
	UFUNCTION()
	void	MeteorWarningStart__(const FMeteor& InMeteor);

	UFUNCTION()
	void	MeteorWarningTimer__(const FMeteor& InMeteor);

	UFUNCTION()
	void	MeteorWarningEnd__();

};
