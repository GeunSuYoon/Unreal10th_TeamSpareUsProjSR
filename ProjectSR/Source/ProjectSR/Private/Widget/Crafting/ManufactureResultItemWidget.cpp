// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Crafting/ManufactureResultItemWidget.h"
#include "CommonHeader/RecipeTable.h"
#include "Data/Item/ItemDataAsset.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"


void UManufactureResultItemWidget::RefreshManufactureResultItemWidget(const FIngredient& InResultItem)
{
    const UItemDataAsset* ItemData = InResultItem.ItemData;
    int32 Quantity = InResultItem.Quantity;

    if (!ItemData)
    {
        // 아마 이게 실행되진 않을거임
        ResultItemIcon->SetBrushFromTexture(nullptr);
        ResultItemIcon->SetBrushTintColor(FLinearColor::Transparent);
        ResultItemName->SetVisibility(ESlateVisibility::Hidden);
        ResultItemQuantity->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        ItemData->Icon.LoadSynchronous();

        ResultItemIcon->SetBrushFromTexture(ItemData->Icon.Get());
        ResultItemIcon->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        ResultItemName->SetText(FText::FromName(ItemData->ItemId));
        ResultItemName->SetVisibility(ESlateVisibility::Visible);
        ResultItemQuantity->SetText(FText::AsNumber(Quantity));
        ResultItemQuantity->SetVisibility(ESlateVisibility::Visible);
    }
}
