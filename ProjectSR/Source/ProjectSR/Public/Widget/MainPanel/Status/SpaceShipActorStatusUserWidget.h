// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipActorStatusUserWidget.generated.h"

class ASpaceShipActor;
class USpaceShipStatusUserWidget;
class ULazerStatusUserWidget;
class UMachineArmStatusUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipActorStatusUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipStatusUserWidget>	SpaceShipStatus = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<ULazerStatusUserWidget>		LazerStatus = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMachineArmStatusUserWidget>	MachineArmStatus = nullptr;

private:
	//void	UpdateStat()
};
