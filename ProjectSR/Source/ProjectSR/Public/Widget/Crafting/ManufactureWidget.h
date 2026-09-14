// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/Crafting/RecipeListWidget.h"
#include "ManufactureWidget.generated.h"

class UCraftingComponent;
class UManufactureResultItemWidget;
class UManufactureIngredientItemWidget;
class UTextBlock;
class UUniformGridPanel;
class UButton;

DECLARE_DELEGATE_OneParam(FOnCraftRequested, FName)

UCLASS()
class PROJECTSR_API UManufactureWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void BindToCraftingComponent(UCraftingComponent* InCraftingComponent);

    UFUNCTION()
    void RefreshManufactureWidget(const FManufactureWidgetDisplayData& InManufactureWidgetRefreshData);

	void	OpenWidget() { this->SetVisibility(ESlateVisibility::Visible); }
	void	CloseWidget() { this->SetVisibility(ESlateVisibility::Collapsed); }

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnCraftButtonClicked__();

public:
    FOnCraftRequested OnCraftRequested;

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

private:
    FName CurrentRecipeId__;

};
