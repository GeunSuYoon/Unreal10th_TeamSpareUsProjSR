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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	Level = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	ItemCollectTime = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	ItemCollectWeight = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;

private:
	UFUNCTION()
	void	UpdateStat__(const FMachineArmStat& InMachineArmStat);

	int32	LevelValue__ = 0;
};
