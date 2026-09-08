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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	Level = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	Damage = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	ReactiveEnergy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;

private:
	UFUNCTION()
	void	UpdateStat__(const FLazerStat& InLazerStat);

	int32	LevelValue__ = 0;
};
