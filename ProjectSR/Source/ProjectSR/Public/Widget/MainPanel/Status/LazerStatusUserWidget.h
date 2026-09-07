// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Blueprint/UserWidget.h"
#include "LazerStatusUserWidget.generated.h"

class ULazerComponent;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API ULazerStatusUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void	BindToLazerComponent(ULazerComponent* InLazer);

protected:
	TObjectPtr<UTextBlock>	Level = nullptr;
	TObjectPtr<UTextBlock>	Damage = nullptr;
	TObjectPtr<UTextBlock>	ReactiveEnergy = nullptr;
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;

private:
	void	UpdateStat__(const FLazerStat& InLazerStat);

	int32	LevelValue__ = 0;
};
