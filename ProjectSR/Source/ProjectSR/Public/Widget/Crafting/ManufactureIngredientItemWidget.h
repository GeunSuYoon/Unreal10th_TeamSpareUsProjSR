// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManufactureIngredientItemWidget.generated.h"

class UImage;
class UTextBlock;
struct FIngredient;

UCLASS()
class PROJECTSR_API UManufactureIngredientItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshManufactureIngredientItemWidget(const FIngredient& InIngredientItem, int32 InCurrentItemCount);

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentItemCount;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> RequiredItemQuantity;

};
