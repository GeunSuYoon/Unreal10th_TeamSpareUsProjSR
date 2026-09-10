// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipStatUserWidget.generated.h"

class ASpaceShipActor;
class USpaceShipEnergyUserWidget;
class USpaceShipDurabilityUserWidget;

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipStatUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipEnergyUserWidget>		SpaceShipEnergy = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipDurabilityUserWidget>	SpaceShipDurability = nullptr;

private:

};
