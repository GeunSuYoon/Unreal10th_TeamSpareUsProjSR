#include "Widget/MainPanel/Upgrade/MachineArmUpgradeUserWidget.h"
#include "Components/TextBlock.h"

void UMachineArmUpgradeUserWidget::BindToDataAsset() { Refresh(); }

void UMachineArmUpgradeUserWidget::RefreshStats(const FShipUpgradePreview& Preview)
{
    NowLevel_ = Preview.CurrentLevel;
    const bool bShowNext = Preview.bHasNextLevel && Preview.Result != EUpgradeResult::InvalidData;
    SetNumber(NextLevel, Preview.NextLevel, bShowNext);
    SetNumber(CurrentTime, Preview.CurrentArm.ItemCollectTime);
    SetNumber(NextTime, Preview.NextArm.ItemCollectTime, bShowNext);
    SetNumber(CurrentWeight, Preview.CurrentArm.ItemCollectWeight);
    SetNumber(NextWeight, Preview.NextArm.ItemCollectWeight, bShowNext);
    SetNumber(CurrentOperationalEnergy, Preview.CurrentArm.OperationalEnergy);
    SetNumber(NextOperationalEnergy, Preview.NextArm.OperationalEnergy, bShowNext);
}
