// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

class UMeteorWarningUserWidget;
class ASpaceShipActor;

// 테스트용
class UMeteorEventUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToPlayer();
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShipActor);
	void	BindToCharacter(ACharacter* InCharacter);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMeteorWarningUserWidget>	MeteoWarningWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMeteorEventUserWidget>		MeteoEventTest;

};
