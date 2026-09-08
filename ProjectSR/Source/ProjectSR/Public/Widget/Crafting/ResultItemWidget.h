// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultItemWidget.generated.h"

class UImage;
class UTextBlock;
struct FIngredient;

UCLASS()
class PROJECTSR_API UResultItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshResultItemWidget(const FIngredient& InResultItem);

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> ResultItemIcon;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> ResultItemQuantity;

};
