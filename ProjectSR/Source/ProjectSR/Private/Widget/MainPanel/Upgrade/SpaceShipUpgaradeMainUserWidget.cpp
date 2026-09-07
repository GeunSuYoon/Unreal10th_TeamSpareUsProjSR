// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Upgrade/SpaceShipUpgaradeMainUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeSelectUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeUserWidget.h"
#include "Widget/MainPanel/Upgrade/LazerUpgradeUserWidget.h"
#include "Widget/MainPanel/Upgrade/MachineArmUpgradeUserWidget.h"

#include "Components/WidgetSwitcher.h"

void USpaceShipUpgaradeMainUserWidget::BindToDataTable()
{
	// 각 자식 위젯에 바인드 함수 콜하기
	this->SpaceShipUpgradeSelect->OnButtonClick.BindUFunction(this, TEXT("SwitchWidget"));
}

void USpaceShipUpgaradeMainUserWidget::BindToBackSpace()
{
	// 메인 패널 위젯의 뒤로가기 버튼에 반응하는 위젯
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

void USpaceShipUpgaradeMainUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SwitchWidget(EUpgradeMenuPage::Select);
}
