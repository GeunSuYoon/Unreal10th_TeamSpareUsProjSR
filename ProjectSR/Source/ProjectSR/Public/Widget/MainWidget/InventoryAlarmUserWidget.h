// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryAlarmUserWidget.generated.h"

class UHorizontalBox;
class UTextBlock;

class UInventoryComponent;

/**
 * 
 */
UCLASS()
class PROJECTSR_API UInventoryAlarmUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToInventory(UInventoryComponent* InInventory);

	void	InventorySlotChange(int32 InCurrentSize, int32 InMaxSize);

	void	InitVisibility();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox>	InventorySlotHorizontalBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentCount = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxCount = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox>	InventoryWeightHorizontalBox = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		CurrentWeight = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>		MaxWeight = nullptr;


private:
	static constexpr float	AlramRate = 0.8f;

};
