// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipStatusUserWidget.generated.h"

class ASpaceShipActor;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipStatusUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	bool	bIsLevelup(int32 InLevel);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	Level = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentDurability = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	MaxDurability = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentEnergy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	MaxEnergy = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	OperationalEnergy = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentCapacity = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentSpeed = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	MaxCapacity = nullptr;

private:
	UFUNCTION()
	void	UpdateStat__(const FSpaceShipStat& InSpaceShipStat);
	UFUNCTION()
	void	UpdateWarehouseSlot__(int32 InCurrentSize, int32 InMaxSize);
	UFUNCTION()
	void	UpdateDurability__(float InCurrentDurability, float InMaxDurability);
	UFUNCTION()
	void	UpdateEnergy__(float InCurrentEnergy, float InMaxEnergy);
	UFUNCTION()
	void	UpdateCurrentDurability__(float InDurability);
	UFUNCTION()
	void	UpdateCurrentEnergy__(float InEnergy);
	UFUNCTION()
	void	UpdateCurrentCapacity__(float InCapacity);

	int32	LevelValue__ = 0;
};
