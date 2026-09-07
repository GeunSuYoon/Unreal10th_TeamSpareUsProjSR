// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Blueprint/UserWidget.h"
#include "MachineArmStatusUserWidget.generated.h"

class UMachineArmComponent;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UMachineArmStatusUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void	BindToMachineArmComponent(UMachineArmComponent* InMachineArm);

protected:
	TObjectPtr<UTextBlock>	Level = nullptr;
	TObjectPtr<UTextBlock>	ItemCollectTime = nullptr;
	TObjectPtr<UTextBlock>	ItemCollectWeight = nullptr;
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;

private:
	void	UpdateStat__(const FMachineArmStat& InMachineArmStat);

	int32	LevelValue__ = 0;
};
