// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Crafting/ManufactureWidget.h"
#include "Widget/Crafting/ManufactureResultItemWidget.h"
#include "Widget/Crafting/ManufactureIngredientItemWidget.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UManufactureWidget::RefreshManufactureWidget(const FRecipeEntry& InRecipeEntry, FIngredientStatusMap InItemStatusMap)
{
    /*
    TODO: 널 체크 하쇼
    */

    RecipeName->SetText(FText::FromName(InRecipeEntry.RecipeId));

    ResultItemGridPanel->ClearChildren();
    for (int i = 0; i < InRecipeEntry.RecipeData.Results.Num(); i++)
    {
        UManufactureResultItemWidget* ManufactureResultItemWidget = CreateWidget<UManufactureResultItemWidget>(GetOwningPlayer(), ManufactureResultItemWidgetClass);
        if (ManufactureResultItemWidget)
        {
            UUniformGridSlot* GridSlot = ResultItemGridPanel->AddChildToUniformGrid(ManufactureResultItemWidget, i, 0);
            if (GridSlot)
            {
                GridSlot->SetHorizontalAlignment(HAlign_Fill);
                GridSlot->SetVerticalAlignment(VAlign_Fill);
            }

            ManufactureResultItemWidget->RefreshManufactureResultItemWidget(InRecipeEntry.RecipeData.Results[i]);
        }
    }

    const int32 MaxColumnCount = 5;
    IngredientItemGridPanel->ClearChildren();
    for (int i = 0; i < InRecipeEntry.RecipeData.Ingredients.Num(); i++)
    {
        UManufactureIngredientItemWidget* ManufactureIngredientItemWidget = CreateWidget<UManufactureIngredientItemWidget>(GetOwningPlayer(), ManufactureIngredientItemWidgetClass);
        if (ManufactureIngredientItemWidget)
        {
            UUniformGridSlot* GridSlot = IngredientItemGridPanel->AddChildToUniformGrid(ManufactureIngredientItemWidget, i / MaxColumnCount, i % MaxColumnCount);
            if (GridSlot)
            {
                GridSlot->SetHorizontalAlignment(HAlign_Fill);
                GridSlot->SetVerticalAlignment(VAlign_Fill);
            }

            ManufactureIngredientItemWidget->RefreshManufactureIngredientItemWidget(
                InRecipeEntry.RecipeData.Ingredients[i],
                InItemStatusMap[InRecipeEntry.RecipeData.Ingredients[i].ItemData->ItemId]);
        }
    }

    SetVisibility(ESlateVisibility::Visible);
}
