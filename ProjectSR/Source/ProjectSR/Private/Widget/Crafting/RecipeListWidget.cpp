// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Crafting/RecipeListWidget.h"
#include "Widget/Crafting/RecipeListElementWidget.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void URecipeListWidget::BindToCraftingComponent(UCraftingComponent* InCraftingComponent)
{
    if (!InCraftingComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[URecipeListWidget::BindToCraftingComponent()] : InCraftingComponent가 nullptr입니다."));
        return;
    }

    CraftingComponent__ = InCraftingComponent;

    RefreshRecipeListWidget();
}

void URecipeListWidget::BindToInventoryCompomnent(UInventoryComponent* InInventoryComponent)
{
    InventoryComponent__ = InInventoryComponent;
}

void URecipeListWidget::RefreshRecipeListWidget()
{
    if (!RecipeGridPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("[URecipeListWidget::RefreshRecipeListWidget()] : RecipeGridPanel이 nullptr입니다."));
        return;
    }

    if (!RecipeListElementWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[URecipeListWidget::RefreshRecipeListWidget()] : RecipeListElementWidgetClass가 nullptr입니다."));
        return;
    }

    if (!CraftingComponent__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[URecipeListWidget::RefreshRecipeListWidget()] : CraftingComponent가 nullptr입니다."));
        return;
    }

    RecipeGridPanel->ClearChildren();

    TArray<FRecipeEntry> RecipeEntries = CraftingComponent__->GetUnlockedRecipeEntries();
    for (int i = 0; i < RecipeEntries.Num(); i++)
    {
        URecipeListElementWidget* RecipeListElementWidget = CreateWidget<URecipeListElementWidget>(GetOwningPlayer(), RecipeListElementWidgetClass);
        if (RecipeListElementWidget)
        {
            UUniformGridSlot* GridSlot = RecipeGridPanel->AddChildToUniformGrid(RecipeListElementWidget, i, 0);
            if (GridSlot)
            {
                GridSlot->SetHorizontalAlignment(HAlign_Fill);
                GridSlot->SetVerticalAlignment(VAlign_Fill);
            }

            RecipeListElementWidget->RefreshRecipeListElementWidget(RecipeEntries[i]);
            RecipeListElementWidget->OnRecipeListElementClicked.Unbind();
            RecipeListElementWidget->OnRecipeListElementClicked.BindUObject(this, &URecipeListWidget::OpenManufactureWidget);
        }
    }
}

void URecipeListWidget::ToggleRecipeListWidget()
{
    if (GetVisibility() == ESlateVisibility::Visible)
    {
        CloseRecipeListWidget();
    }
    else
    {
        OpenRecipeListWidget();
    }
}

void URecipeListWidget::OpenRecipeListWidget()
{
    if (!CraftingComponent__.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[URecipeListWidget::OpenRecipeListWidget()] : CraftingComponent가 nullptr입니다."));
        return;
    }

    RefreshRecipeListWidget();

    SetVisibility(ESlateVisibility::Visible);

    // DELETE ME
    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        FInputModeUIOnly InputModeUI;
        InputModeUI.SetWidgetToFocus(TakeWidget());

        PC->SetInputMode(InputModeUI);
        PC->SetShowMouseCursor(true);
    }
}

void URecipeListWidget::CloseRecipeListWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void URecipeListWidget::OpenManufactureWidget(FName InRecipeId)
{
    UE_LOG(LogTemp, Warning, TEXT("HELL"));
    // InRecipeId받아서 이 제작법에 대한 정보와, 이거에 필요한 아이템 재료들 현황을 불러와야됨.
    // 인벤토리 컴포넌트 있으니까 이거 사용해서.

    FRecipeEntry Entry = CraftingComponent__->GetRecipeEntry(InRecipeId);
    TMap<FName, int32> InItemStatusMap;

    for (int i = 0; i < Entry.RecipeData.Ingredients.Num(); i++)
    {
        InItemStatusMap.Add(Entry.RecipeData.Ingredients[i].ItemData->ItemId, InventoryComponent__->GetTotalItemCount(Entry.RecipeData.Ingredients[i].ItemData));
    }

    OnManufactureWidgetOpened.ExecuteIfBound(Entry, InItemStatusMap);
}

void URecipeListWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);
    CloseRecipeListWidget();
}
