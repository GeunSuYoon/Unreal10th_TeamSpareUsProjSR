// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Component/CraftingComponent.h"
#include "RecipeListWidget.generated.h"

class UInventoryComponent;
class URecipeListElementWidget;
class UUniformGridPanel;

using FIngredientStatusMap = TMap<FName, int32>;
DECLARE_DELEGATE_TwoParams(FOnManufactureWidgetOpened, const FRecipeEntry&, FIngredientStatusMap)

UCLASS()
class PROJECTSR_API URecipeListWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void BindToCraftingComponent(UCraftingComponent* InCraftingComponent);
    void BindToInventoryCompomnent(UInventoryComponent* InInventoryComponent); // DELETE ME
    void RefreshRecipeListWidget();
    UFUNCTION(BlueprintCallable)
    void ToggleRecipeListWidget();
    void OpenRecipeListWidget();
    void CloseRecipeListWidget();

    void OpenManufactureWidget(FName InRecipeId);

protected:
    virtual void NativeConstruct() override;

public:
    FOnManufactureWidgetOpened OnManufactureWidgetOpened;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<URecipeListElementWidget> RecipeListElementWidgetClass;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> RecipeGridPanel;

private:
    TWeakObjectPtr<UCraftingComponent> CraftingComponent__ = nullptr;
    TWeakObjectPtr<UInventoryComponent> InventoryComponent__ = nullptr;

};
