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

void UManufactureWidget::BindToCraftingComponent(UCraftingComponent* InCraftingComponent)
{
    OnCraftRequested.BindUObject(InCraftingComponent, &UCraftingComponent::HandleCraftRequested);
    InCraftingComponent->OnManufactureWidgetOpened.AddDynamic(this, &UManufactureWidget::RefreshManufactureWidget);
	this->CloseWidget();
}

void UManufactureWidget::RefreshManufactureWidget(const FManufactureWidgetDisplayData& InManufactureWidgetRefreshData)
{
    CurrentRecipeId__ = InManufactureWidgetRefreshData.RecipeEntry.RecipeId;
    FRecipeTableRow RecipeData = InManufactureWidgetRefreshData.RecipeEntry.RecipeData;
    TMap<FName, int32> IngredientStatusMap = InManufactureWidgetRefreshData.IngredientStatusMap;


    RecipeName->SetText(FText::FromName(InManufactureWidgetRefreshData.RecipeEntry.RecipeName));

    ResultItemGridPanel->ClearChildren();
    for (int i = 0; i < RecipeData.Results.Num(); i++)
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

            ManufactureResultItemWidget->RefreshManufactureResultItemWidget(RecipeData.Results[i]);
        }
    }

    const int32 MaxColumnCount = 5;
    IngredientItemGridPanel->ClearChildren();
    for (int i = 0; i < RecipeData.Ingredients.Num(); i++)
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
                RecipeData.Ingredients[i],
                IngredientStatusMap[RecipeData.Ingredients[i].ItemData->ItemId]);
        }
    }

    // 제작 가능 여부로 제작 버튼 활성화or비활성화
    CraftButton->SetIsEnabled(InManufactureWidgetRefreshData.bHasEnoughIngredients);

    SetVisibility(ESlateVisibility::Visible);
}

void UManufactureWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CraftButton->OnClicked.AddDynamic(this, &UManufactureWidget::OnCraftButtonClicked__);
}

void UManufactureWidget::OnCraftButtonClicked__()
{
    OnCraftRequested.ExecuteIfBound(CurrentRecipeId__);
}
