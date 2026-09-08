// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetStackHostInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidgetStackHostInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSR_API IWidgetStackHostInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetStackHost")
	bool	CloseTopWidget();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetStackHost")
	void	ClearStackWidget();

};
