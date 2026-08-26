// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatComponentInterface.generated.h"

class UStatComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSR_API IStatComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	UStatComponent*	GetStatComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void	IncreaseHP(float InHP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void	DecreaseHP(float InHP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void	ConsumOxigen(float InOxigen);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat")
	void	RecoverOxigen(float InOxigen);
};
