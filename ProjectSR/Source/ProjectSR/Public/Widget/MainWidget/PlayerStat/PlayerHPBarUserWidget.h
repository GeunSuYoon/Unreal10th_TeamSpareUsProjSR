// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHPBarUserWidget.generated.h"

class UTextBlock;
class UProgressBar;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class PROJECTSR_API UPlayerHPBarUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToPlayer(APlayerCharacter* InPlayer);

	UFUNCTION()
	void	HPChange(float InCurrentHP, float InMaxHP);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar>	HPProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentHPText = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxHPText = nullptr;
};
