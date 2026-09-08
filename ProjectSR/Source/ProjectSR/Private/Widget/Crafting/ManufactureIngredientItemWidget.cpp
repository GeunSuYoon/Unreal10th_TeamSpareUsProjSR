// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Crafting/ManufactureIngredientItemWidget.h"
#include "CommonHeader/RecipeTable.h"
#include "Data/Item/ItemDataAsset.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UManufactureIngredientItemWidget::RefreshManufactureIngredientItemWidget(const FIngredient& InIngredientItem, int32 InCurrentItemCount)
{
    const UItemDataAsset* ItemData = InIngredientItem.ItemData;
    int32 RequiredQuantity = InIngredientItem.Quantity;

    if (!ItemData)
    {
        // 아마 이게 실행되진 않을거임
        ItemIcon->SetBrushFromTexture(nullptr);
        ItemIcon->SetBrushTintColor(FLinearColor::Transparent);
        CurrentItemCount->SetVisibility(ESlateVisibility::Hidden);
        RequiredItemQuantity->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        ItemData->Icon.LoadSynchronous();

        ItemIcon->SetBrushFromTexture(ItemData->Icon.Get());
        ItemIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        CurrentItemCount->SetText(FText::AsNumber(InCurrentItemCount));
        CurrentItemCount->SetVisibility(ESlateVisibility::Visible);
        RequiredItemQuantity->SetText(FText::AsNumber(RequiredQuantity));
        RequiredItemQuantity->SetVisibility(ESlateVisibility::Visible);
    }
}
