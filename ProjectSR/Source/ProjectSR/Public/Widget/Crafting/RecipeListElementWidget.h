// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/CraftingComponent.h"
#include "RecipeListElementWidget.generated.h"

class UResultItemWidget;
class UTextBlock;
class UUniformGridPanel;

DECLARE_DELEGATE_OneParam(FOnRecipeListElementClicked, FName)

UCLASS()
class PROJECTSR_API URecipeListElementWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshRecipeListElementWidget(const FRecipeEntry& InRecipeEntry);

protected:
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
    FOnRecipeListElementClicked OnRecipeListElementClicked;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UResultItemWidget> ResultItemWidgetClass;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> RecipeName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> ResultItemGridPanel;

private:
    FRecipeEntry RecipeEntry__;

};
