// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatUserWidget.generated.h"

class APlayerCharacter;
class UPlayerHPBarUserWidget;
class USpaceSuitOxygenUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UPlayerStatUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void	BindToPlayer(APlayerCharacter* InPlayer);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPlayerHPBarUserWidget>		PlayerHPBar = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceSuitOxygenUserWidget>	SpaceSuitOxygen = nullptr;

};
