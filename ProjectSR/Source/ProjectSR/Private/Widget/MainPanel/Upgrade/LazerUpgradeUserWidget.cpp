#include "Widget/MainPanel/Upgrade/LazerUpgradeUserWidget.h"
#include "Components/TextBlock.h"

void ULazerUpgradeUserWidget::BindToDataAsset() { Refresh(); }

void ULazerUpgradeUserWidget::RefreshStats(const FShipUpgradePreview& Preview)
{
    NowLevel_ = Preview.CurrentLevel;
    const bool bShowNext = Preview.bHasNextLevel && Preview.Result != EUpgradeResult::InvalidData;
    SetNumber(NextLevel, Preview.NextLevel, bShowNext);
    SetNumber(CurrentDurability, Preview.CurrentLazer.Damage);
    SetNumber(NextDurability, Preview.NextLazer.Damage, bShowNext);
    SetNumber(CurrentReactiveEnergy, Preview.CurrentLazer.ReactiveEnergy);
    SetNumber(NextReactiveEnergy, Preview.NextLazer.ReactiveEnergy, bShowNext);
    SetNumber(CurrentOperationalEnergy, Preview.CurrentLazer.OperationalEnergy);
    SetNumber(NextOperationalEnergy, Preview.NextLazer.OperationalEnergy, bShowNext);
}
