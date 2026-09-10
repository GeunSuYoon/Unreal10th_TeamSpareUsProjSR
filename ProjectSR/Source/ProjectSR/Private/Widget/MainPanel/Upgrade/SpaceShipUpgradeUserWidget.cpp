#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeUserWidget.h"
#include "Components/TextBlock.h"

void USpaceShipUpgradeUserWidget::BindToDataAsset() { Refresh(); }

void USpaceShipUpgradeUserWidget::RefreshStats(const FShipUpgradePreview& Preview)
{
    NowLevel_ = Preview.CurrentLevel;
    const bool bShowNext = Preview.bHasNextLevel && Preview.Result != EUpgradeResult::InvalidData;
    SetNumber(NextLevel, Preview.NextLevel, bShowNext);
    SetNumber(CurrentDurability, Preview.CurrentShip.MaxDurability);
    SetNumber(NextDurability, Preview.NextShip.MaxDurability, bShowNext);
    SetNumber(CurrentEnergy, Preview.CurrentShip.MaxEnergy);
    SetNumber(NextEnergy, Preview.NextShip.MaxEnergy, bShowNext);
    SetNumber(CurrentSize, Preview.CurrentShip.MaxCapacity);
    SetNumber(NextSize, Preview.NextShip.MaxCapacity, bShowNext);
}
