// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipStatusUserWidget.generated.h"

class ASpaceShipActor;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipStatusUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	bool	bIsLevelup(int32 InLevel);

protected:
	TObjectPtr<UTextBlock>	Level = nullptr;
	TObjectPtr<UTextBlock>	CurrentDurability = nullptr;
	TObjectPtr<UTextBlock>	MaxDurability = nullptr;
	TObjectPtr<UTextBlock>	CurrentEnergy = nullptr;
	TObjectPtr<UTextBlock>	MaxEnergy = nullptr;
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;
	TObjectPtr<UTextBlock>	CurrentCapacity = nullptr;
	TObjectPtr<UTextBlock>	MaxCapacity = nullptr;

private:
	void	UpdateStat__(const FSpaceShipStat& InSpaceShipStat);
	void	UpdateCurrentDurability__(float InDurability);
	void	UpdateCurrentEnergy__(float InEnergy);
	void	UpdateCurrentCapacity__(float InCapacity);

	int32	LevelValue__ = 0;
};
