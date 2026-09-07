// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/Crafting/RecipeListWidget.h"
#include "ManufactureWidget.generated.h"

struct FRecipeEntry;
class UManufactureResultItemWidget;
class UManufactureIngredientItemWidget;
class UTextBlock;
class UUniformGridPanel;
class UButton;

UCLASS()
class PROJECTSR_API UManufactureWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshManufactureWidget(const FRecipeEntry& InRecipeEntry, FIngredientStatusMap InItemStatusMap);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UManufactureResultItemWidget> ManufactureResultItemWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UManufactureIngredientItemWidget> ManufactureIngredientItemWidgetClass;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> RecipeName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> ResultItemGridPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> IngredientItemGridPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> CraftButton;

};
