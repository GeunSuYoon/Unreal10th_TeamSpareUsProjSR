#include "Widget/MainPanel/Upgrade/ShipUpgradeDetailUserWidget.h"
#include "Widget/Crafting/ManufactureIngredientItemWidget.h"
#include "Data/Item/ItemDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UShipUpgradeDetailUserWidget::EnsureControls()
{
    if (!WidgetTree || (UpgradeButton && IngredientList && UpgradeMessage)) return;
    // Existing WBP assets work immediately; designers may supply the optional named controls.
    UWidget* OriginalRoot = WidgetTree->RootWidget;
    auto* Layout = WidgetTree->ConstructWidget<UVerticalBox>();
    WidgetTree->RootWidget = Layout;
    if (OriginalRoot)
    {
        auto* RootSlot = Layout->AddChildToVerticalBox(OriginalRoot);
        RootSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
    }
    if (!IngredientList)
    {
        IngredientList = WidgetTree->ConstructWidget<UVerticalBox>();
        Layout->AddChild(IngredientList);
    }
    if (!UpgradeMessage)
    {
        UpgradeMessage = WidgetTree->ConstructWidget<UTextBlock>();
        Layout->AddChild(UpgradeMessage);
    }
    if (!UpgradeButton)
    {
        UpgradeButton = WidgetTree->ConstructWidget<UButton>();
        auto* Label = WidgetTree->ConstructWidget<UTextBlock>();
        Label->SetText(FText::FromString(TEXT("업그레이드")));
        UpgradeButton->AddChild(Label);
        Layout->AddChild(UpgradeButton);
    }
}

void UShipUpgradeDetailUserWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    EnsureControls();
    if (UpgradeButton) UpgradeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleUpgradeClicked);
}
void UShipUpgradeDetailUserWidget::Subscribe()
{
    if (IsValid(UpgradeComponent))
        UpgradeComponent->OnUpgradeStateChanged.AddUniqueDynamic(this, &ThisClass::Refresh);
}
void UShipUpgradeDetailUserWidget::NativeConstruct()
{
    Super::NativeConstruct();
    Subscribe();
    Refresh();
}
void UShipUpgradeDetailUserWidget::NativeDestruct()
{
    if (IsValid(UpgradeComponent))
        UpgradeComponent->OnUpgradeStateChanged.RemoveDynamic(this, &ThisClass::Refresh);
    Super::NativeDestruct();
}
void UShipUpgradeDetailUserWidget::BindToUpgradeComponent(USpaceShipUpgradeComponent* InComponent)
{
    if (IsValid(UpgradeComponent))
        UpgradeComponent->OnUpgradeStateChanged.RemoveDynamic(this, &ThisClass::Refresh);
    UpgradeComponent = InComponent;
    Subscribe();
    Refresh();
}
void UShipUpgradeDetailUserWidget::SetNumber(UTextBlock* Text, float Value, bool bAvailable)
{
    if (Text) Text->SetText(bAvailable ? FText::AsNumber(Value) : FText::FromString(TEXT("—")));
}
FText UShipUpgradeDetailUserWidget::GetResultText(EUpgradeResult Result, int32 NextLevel)
{
    switch (Result)
    {
    case EUpgradeResult::Success: return FText::GetEmpty();
    case EUpgradeResult::MaxLevel: return FText::FromString(TEXT("최대 레벨입니다."));
    case EUpgradeResult::NotEnoughIngredients: return FText::FromString(TEXT("재료가 부족합니다."));
    case EUpgradeResult::RequiresHigherShipLevel:
        return FText::Format(FText::FromString(TEXT("우주선 레벨 {0} 필요")), FText::AsNumber(static_cast<int64>(NextLevel) + 1));
    case EUpgradeResult::Busy: return FText::FromString(TEXT("업그레이드 중입니다."));
    default: return FText::FromString(TEXT("업그레이드 데이터와 연결을 확인하세요."));
    }
}
void UShipUpgradeDetailUserWidget::Refresh()
{
    const FShipUpgradePreview Preview = IsValid(UpgradeComponent)
        ? UpgradeComponent->GetPreview(GetUpgradeTarget()) : FShipUpgradePreview();
    RefreshStats(Preview);
    if (UpgradeButton) UpgradeButton->SetIsEnabled(Preview.Result == EUpgradeResult::Success);
    if (UpgradeMessage) UpgradeMessage->SetText(GetResultText(Preview.Result, Preview.NextLevel));
    if (!IngredientList) return;
    IngredientList->ClearChildren();
    if (!Preview.bHasNextLevel || Preview.Result == EUpgradeResult::InvalidData) return;
    for (const FIngredient& Ingredient : Preview.Ingredients)
    {
        const int32 Owned = UpgradeComponent->GetOwnedIngredientCount(Ingredient.ItemData);
        if (IngredientWidgetClass && GetOwningPlayer())
        {
            auto* Row = CreateWidget<UManufactureIngredientItemWidget>(GetOwningPlayer(), IngredientWidgetClass);
            if (Row)
            {
                IngredientList->AddChild(Row);
                Row->RefreshManufactureIngredientItemWidget(Ingredient, Owned);
            }
        }
        else
        {
            auto* Row = WidgetTree->ConstructWidget<UTextBlock>();
            Row->SetText(FText::Format(FText::FromString(TEXT("{0}: {1} / {2}")),
                Ingredient.ItemData->DisplayName.IsEmpty() ? FText::FromName(Ingredient.ItemData->ItemId) : Ingredient.ItemData->DisplayName,
                FText::AsNumber(Owned), FText::AsNumber(Ingredient.Quantity)));
            Row->SetColorAndOpacity(FSlateColor(Owned >= Ingredient.Quantity ? FLinearColor::White : FLinearColor(1.f, .25f, .25f)));
            IngredientList->AddChild(Row);
        }
    }
}
void UShipUpgradeDetailUserWidget::HandleUpgradeClicked()
{
    if (!IsValid(UpgradeComponent)) { Refresh(); return; }
    const EUpgradeResult Result = UpgradeComponent->TryUpgrade(GetUpgradeTarget());
    Refresh();
    if (UpgradeMessage)
    {
        if (Result == EUpgradeResult::Success)
            UpgradeMessage->SetText(FText::FromString(TEXT("업그레이드 완료")));
        else
            UpgradeMessage->SetText(GetResultText(Result, UpgradeComponent->GetPreview(GetUpgradeTarget()).NextLevel));
    }
}
