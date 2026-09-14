// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/MainPanelHomeUserWidget.h"

#include "Components/Button.h"
#include "Framework/SurvivalLoopActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"

void UMainPanelHomeUserWidget::SpaceShipStatusSelect()
{
	this->OnMainPanelHomeSelect.ExecuteIfBound(EMainPanelMenuPage::SpaceShipStatus);
}

void UMainPanelHomeUserWidget::WarehouseSelect()
{
	this->OnMainPanelHomeSelect.ExecuteIfBound(EMainPanelMenuPage::Warehouse);
}

void UMainPanelHomeUserWidget::MeteorEventSelect()
{
	this->OnMainPanelHomeSelect.ExecuteIfBound(EMainPanelMenuPage::MeteorEvent);
}

void UMainPanelHomeUserWidget::UpgradeSelect()
{
	this->OnMainPanelHomeSelect.ExecuteIfBound(EMainPanelMenuPage::Upgrade);
}

void UMainPanelHomeUserWidget::ItemCraftSelect()
{
	this->OnMainPanelHomeSelect.ExecuteIfBound(EMainPanelMenuPage::ItemCraft);
}

void UMainPanelHomeUserWidget::FinishDay()
{
	if (UWorld* World = GetWorld())
	{
		if (USpaceSalvageWorldSubsystem* Salvage = World->GetSubsystem<USpaceSalvageWorldSubsystem>())
		{
			if (ASurvivalLoopActor* SurvivalLoop = Salvage->GetSurvivalLoop())
			{
				SurvivalLoop->FinishDay();
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[UMainPanelHomeUserWidget::FinishDay] Active SurvivalLoopActor was not found."));
}

void UMainPanelHomeUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	this->SpaceShipStatusButton->OnClicked.AddDynamic(this, &UMainPanelHomeUserWidget::SpaceShipStatusSelect);
	this->WarehouseButton->OnClicked.AddDynamic(this, &UMainPanelHomeUserWidget::WarehouseSelect);
	this->MeteorEventButton->OnClicked.AddDynamic(this, &UMainPanelHomeUserWidget::MeteorEventSelect);
	this->UpgradeButton->OnClicked.AddDynamic(this, &UMainPanelHomeUserWidget::UpgradeSelect);
	this->ItemCraftButton->OnClicked.AddDynamic(this, &UMainPanelHomeUserWidget::ItemCraftSelect);
	this->FinishDayButton->OnClicked.AddUniqueDynamic(this, &UMainPanelHomeUserWidget::FinishDay);
}
