// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RecipeTable.generated.h"

USTRUCT(BlueprintType)
struct PROJECTSR_API FIngredient
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FPrimaryAssetId ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Quantity = 1;

};

USTRUCT(BlueprintType)
struct PROJECTSR_API FRecipeTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FIngredient> Ingredients;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FIngredient> Results;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLockedByDefault = true;

};
