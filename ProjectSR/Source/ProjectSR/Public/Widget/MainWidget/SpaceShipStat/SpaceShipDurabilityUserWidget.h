// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipDurabilityUserWidget.generated.h"

class ASpaceShipActor;
class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipDurabilityUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	UFUNCTION()
	void	DurabilityChange(float InCurrentDurability, float InMaxDurability);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar>	DurabilityProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentDurabilityText = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxDurabilityText = nullptr;

};
