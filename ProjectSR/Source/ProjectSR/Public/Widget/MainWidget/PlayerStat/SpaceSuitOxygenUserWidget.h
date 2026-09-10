// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "SpaceSuitOxygenUserWidget.generated.h"

class UTextBlock;
class UProgressBar;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceSuitOxygenUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void	BindToPlayer(APlayerCharacter* InPlayer);

	UFUNCTION()
	void	OxygenChange(float InCurrentOxygen, float InMaxOxygen);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar>	OxygenProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentOxygenText = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxOxygenText = nullptr;
};
