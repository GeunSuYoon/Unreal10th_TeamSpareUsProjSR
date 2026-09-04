// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SRMainHUD.generated.h"

class APlayerCharacter;
class UMainUserWidget;
class ASpaceShipActor;
/**
 * 
 */
UCLASS()
class PROJECTSR_API ASRMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void	BeginPlay() override;

	void	RegisterSpaceShip(ASpaceShipActor* InSpaceShipActor);
    void	RegisterPlayerCharacter(APlayerCharacter* InPlayerCharacter);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget>	MainUserWidgetClass_ = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMainUserWidget>		MainUserWidgetInstance_ = nullptr;

};
