// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Upgrade/SpaceShipUpgradeSelectUserWidget.h"
#include "Components/Button.h"

void USpaceShipUpgradeSelectUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	this->SpaceShipButton->OnClicked.AddDynamic(this, &USpaceShipUpgradeSelectUserWidget::OnSpaceShipButtonClick);
	this->LazerButton->OnClicked.AddDynamic(this, &USpaceShipUpgradeSelectUserWidget::OnLazerButtonClick);
	this->MachineArmButton->OnClicked.AddDynamic(this, &USpaceShipUpgradeSelectUserWidget::OnMachineArmButtonClick);
}

void USpaceShipUpgradeSelectUserWidget::OnSpaceShipButtonClick()
{
	OnMainPanelUpgradeHomeSelect.ExecuteIfBound(EUpgradeMenuPage::SpaceShip);
}

void USpaceShipUpgradeSelectUserWidget::OnLazerButtonClick()
{
	OnMainPanelUpgradeHomeSelect.ExecuteIfBound(EUpgradeMenuPage::Lazer);
}

void USpaceShipUpgradeSelectUserWidget::OnMachineArmButtonClick()
{
	OnMainPanelUpgradeHomeSelect.ExecuteIfBound(EUpgradeMenuPage::MachineArm);
}
