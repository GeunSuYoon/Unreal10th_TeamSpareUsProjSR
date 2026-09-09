// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemAction/ItemAction.h"
#include "PlateItemAction.generated.h"

UCLASS()
class PROJECTSR_API UPlateItemAction : public UItemAction
{
    GENERATED_BODY()

public:
    virtual void ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DurabilityAmount = 0.0f;

};
