// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Upgrade/SpaceShipUpgaradeMainUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeSelectUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeUserWidget.h"
#include "Widget/MainPanel/Upgrade/LazerUpgradeUserWidget.h"
#include "Widget/MainPanel/Upgrade/MachineArmUpgradeUserWidget.h"

#include "Components/WidgetSwitcher.h"

void USpaceShipUpgaradeMainUserWidget::BindToDataTable()
{
	RefreshUpgradePages();
}

void USpaceShipUpgaradeMainUserWidget::BindToUpgradeComponent(USpaceShipUpgradeComponent* InComponent)
{
	if (SpaceShipUpgrade) SpaceShipUpgrade->BindToUpgradeComponent(InComponent);
	if (LazerUpgrade) LazerUpgrade->BindToUpgradeComponent(InComponent);
	if (MachineArmUpgrade) MachineArmUpgrade->BindToUpgradeComponent(InComponent);
}

void USpaceShipUpgaradeMainUserWidget::RefreshUpgradePages()
{
	if (SpaceShipUpgrade) SpaceShipUpgrade->Refresh();
	if (LazerUpgrade) LazerUpgrade->Refresh();
	if (MachineArmUpgrade) MachineArmUpgrade->Refresh();
}

//void USpaceShipUpgaradeMainUserWidget::BindToBackSpace()
//{
//	// 메인 패널 위젯의 뒤로가기 버튼에 반응하는 위젯
//}

bool	USpaceShipUpgaradeMainUserWidget::CloseTopWidget_Implementation()
{
	if (!ensure(this->SpaceShipUpgradeSwitcher))
	{
		return (true);
	}
	if (this->StackSize__ == 1)
	{
		return (true);
	}
	this->StackSize__--;
	this->OpenWidgetStack__.Pop();
	this->SwitchWidget(this->StackSize__ - 1);
	return (false);
}

void	USpaceShipUpgaradeMainUserWidget::ClearStackWidget_Implementation()
{
	while (!IWidgetStackHostInterface::Execute_CloseTopWidget(this))
	{	}
}

void USpaceShipUpgaradeMainUserWidget::SwitchWidget(EUpgradeMenuPage InPage)
{
	int32	InIndex = static_cast<int32>(InPage);

	if (!this->SpaceShipUpgradeSwitcher ||
		InIndex >= SpaceShipUpgradeSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceShipUpgaradeMainUserWidget::SwitchWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	SpaceShipUpgradeSwitcher->SetActiveWidgetIndex(InIndex);
}

void USpaceShipUpgaradeMainUserWidget::SwitchWidget(int32 InIndex)
{
	if (!this->SpaceShipUpgradeSwitcher ||
		InIndex >= SpaceShipUpgradeSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceShipUpgaradeMainUserWidget::SwitchWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	SpaceShipUpgradeSwitcher->SetActiveWidgetIndex(InIndex);
}

void USpaceShipUpgaradeMainUserWidget::SwitchTargetWidget(EUpgradeMenuPage InPage)
{
	RefreshUpgradePages();
	int32	InIndex = static_cast<int32>(InPage);

	if (!this->SpaceShipUpgradeSwitcher ||
		InIndex >= SpaceShipUpgradeSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceShipUpgaradeMainUserWidget::SwitchTargetWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	this->SpaceShipUpgradeSwitcher->SetActiveWidgetIndex(InIndex);
	this->OpenWidgetStack__.Add(SpaceShipUpgradeSwitcher->GetWidgetAtIndex(static_cast<int32>(InPage)));
	this->StackSize__++;
}

void USpaceShipUpgaradeMainUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(this->SpaceShipUpgradeSwitcher && this->SpaceShipUpgradeSelect))
	{
		SwitchTargetWidget(EUpgradeMenuPage::Home);
		this->SpaceShipUpgradeSelect->OnMainPanelUpgradeHomeSelect.BindUFunction(this, TEXT("SwitchTargetWidget"));
	}
}
