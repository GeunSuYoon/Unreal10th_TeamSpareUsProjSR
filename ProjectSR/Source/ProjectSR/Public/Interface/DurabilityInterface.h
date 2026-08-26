// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DurabilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDurabilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSR_API IDurabilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Durability")
	void	RepairDurability(float InDurability);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Durability")
	void	ConsumDurability(float InDurability);
};
