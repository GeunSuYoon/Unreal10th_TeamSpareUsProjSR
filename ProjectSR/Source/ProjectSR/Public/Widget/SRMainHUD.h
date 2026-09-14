// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SRMainHUD.generated.h"

class APlayerCharacter;
class UMainUserWidget;
class ASpaceShipActor;
class ASurvivalLoopActor;
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
	void	RegisterSurvivalLoop(ASurvivalLoopActor* InSurvivalLoop);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMainUserWidget>	MainUserWidgetClass_ = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMainUserWidget>		MainUserWidgetInstance_ = nullptr;

};
