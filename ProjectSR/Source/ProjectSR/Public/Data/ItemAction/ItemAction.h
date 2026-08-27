// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemAction.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class PROJECTSR_API UItemAction : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "ItemAction")
    void ExecuteItemAction(AActor* InInstigator, AActor* InTarget);

    virtual void ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget) {}
};
