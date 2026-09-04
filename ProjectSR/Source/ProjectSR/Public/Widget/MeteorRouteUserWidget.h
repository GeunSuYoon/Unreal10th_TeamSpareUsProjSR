// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeteorRouteUserWidget.generated.h"

//class USizeBox;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UMeteorRouteUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void	SetVisibleToggle(bool bInIsVisible);

//protected:

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	//TObjectPtr<USizebox>	RemainedSecond;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation>	ArrowFlowAnim;

protected:
	virtual void	NativeConstruct() override;

private:
	bool	bIsVisible = true;

};
