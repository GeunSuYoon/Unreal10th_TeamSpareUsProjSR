// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ManufactureResultItemWidget.generated.h"

class UImage;
class UTextBlock;
struct FIngredient;

UCLASS()
class PROJECTSR_API UManufactureResultItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshManufactureResultItemWidget(const FIngredient& InResultItem);

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> ResultItemIcon;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ResultItemName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ResultItemQuantity;

};
