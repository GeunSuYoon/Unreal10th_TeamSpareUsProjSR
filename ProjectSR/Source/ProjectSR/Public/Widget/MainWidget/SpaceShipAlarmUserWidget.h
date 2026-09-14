// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShipAlarmUserWidget.generated.h"

class ASpaceShipActor;

class UHorizontalBox;
class UTextBlock;
class USoundBase;

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipAlarmUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	UFUNCTION()
	void	SpaceShipEnergyChange(float InCurrentEnergy, float InMaxEnergy);
	UFUNCTION()
	void	SpaceShipDurabilityChange(float InCurrentDurability, float InMaxDurability);
	UFUNCTION()
	void	RequiredDurabilityChange(float InRequiredDurability);

	void	InitVisibility();
	void	CheckChildVisibility();
	void	CheckDurability();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox>	EnergyAlarmHorizontalBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentEnergy = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		OperationalEnergy = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox>	DurabilityAlarmHorizontalBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentDurability = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		RequiredDurability = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Alarm|Audio")
	TObjectPtr<USoundBase> EnergyAlarmSFX = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Alarm|Audio")
	TObjectPtr<USoundBase> DurabilityAlarmSFX = nullptr;


private:
	UPROPERTY()
	TObjectPtr<ASpaceShipActor> BoundSpaceShip__ = nullptr;

	float	RequiredDurability__ = 0.0f;
	float	CurrentDurability__ = 0.0f;

};
