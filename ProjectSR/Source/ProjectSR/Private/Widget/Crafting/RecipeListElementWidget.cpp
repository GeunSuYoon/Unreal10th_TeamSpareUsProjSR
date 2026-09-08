// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Crafting/RecipeListElementWidget.h"
#include "Widget/Crafting/ResultItemWidget.h"
#include "CommonHeader/RecipeTable.h"

#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void URecipeListElementWidget::RefreshRecipeListElementWidget(const FRecipeEntry& InRecipeEntry)
{
    RecipeEntry__ = InRecipeEntry;

    RecipeName->SetText(FText::FromName(RecipeEntry__.RecipeId));

    // 결과물 아이템 UI 갱신
    ResultItemGridPanel->ClearChildren();

    for (int i = 0; i < RecipeEntry__.RecipeData.Results.Num(); i++)
    {
        UResultItemWidget* ResultItemWidget = CreateWidget<UResultItemWidget>(GetOwningPlayer(), ResultItemWidgetClass);
        if (ResultItemWidget)
        {
            UUniformGridSlot* GridSlot = ResultItemGridPanel->AddChildToUniformGrid(ResultItemWidget, 0, i);
            if (GridSlot)
            {
                GridSlot->SetHorizontalAlignment(HAlign_Fill);
                GridSlot->SetVerticalAlignment(VAlign_Fill);
            }
        }

        ResultItemWidget->RefreshResultItemWidget(RecipeEntry__.RecipeData.Results[i]);
    }
}

FReply URecipeListElementWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    OnRecipeListElementClicked.ExecuteIfBound(RecipeEntry__.RecipeId);

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}
