// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipEnergyUserWidget.generated.h"

class ASpaceShipActor;

class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipEnergyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	UFUNCTION()
	void	EnergyChange(float InCurrentEnergy, float InMaxEnergy);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar>	EnergyProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentEnergyText = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxEnergyText = nullptr;

};
