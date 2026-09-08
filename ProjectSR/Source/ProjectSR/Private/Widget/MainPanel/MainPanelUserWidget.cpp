// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/MainPanelUserWidget.h"
#include "Widget/MainPanel/MainPanelHomeUserWidget.h"
#include "Widget/MainPanel/Status/SpaceShipActorStatusUserWidget.h"
//#include "Widget/MainPanel/Status/SpaceShipActorStatusUserWidget.h" // 창고 자리
#include "Widget/MeteorEventUserWidget.h"
#include "Widget/MainPanel/Upgrade/SpaceShipUpgaradeMainUserWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"

void	UMainPanelUserWidget::OpenSelfWidget_Implementation()
{
	this->SetVisibility(ESlateVisibility::Visible);
}

void	UMainPanelUserWidget::CloseSelfWidget_Implementation()
{
	IWidgetStackHostInterface::Execute_ClearStackWidget(this);
	this->SetVisibility(ESlateVisibility::Collapsed);
}

bool	UMainPanelUserWidget::CloseTopWidget_Implementation()
{
	if (!ensure(this->MainPanelSwitcher))
	{
		return (true);
	}
	if (this->StackSize__ == 1)
	{
		return (true);
	}
	if (this->OpenWidgetStack__.Last()->GetClass()->ImplementsInterface(UWidgetStackHostInterface::StaticClass()))
	{
		if (!IWidgetStackHostInterface::Execute_CloseTopWidget(this->OpenWidgetStack__.Last()))
			return (false);
	}
	this->StackSize__--;
	this->OpenWidgetStack__.Pop();
	this->SwitchWidget(this->StackSize__ - 1);
	return (false);
}

void UMainPanelUserWidget::ClearStackWidget_Implementation()
{
	while (!IWidgetStackHostInterface::Execute_CloseTopWidget(this))
	{	}
}

void	UMainPanelUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	this->SpaceShipStatus->BindToSpaceShip(InSpaceShip);
	this->MeteoEvent->BindToSpaceShip(InSpaceShip);
}

void	UMainPanelUserWidget::SwitchWidget(EMainPanelMenuPage InPage)
{
	int32	InIndex = static_cast<int32>(InPage);

	if (!this->MainPanelSwitcher ||
		InIndex >= MainPanelSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::SwitchWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	MainPanelSwitcher->SetActiveWidgetIndex(InIndex);
}

void UMainPanelUserWidget::SwitchWidget(int32 InIndex)
{
	if (!this->MainPanelSwitcher ||
		InIndex >= MainPanelSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::SwitchWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	MainPanelSwitcher->SetActiveWidgetIndex(InIndex);
}

void UMainPanelUserWidget::SwitchTargetWidget(EMainPanelMenuPage InPage)
{
	int32	InIndex = static_cast<int32>(InPage);

	if (!this->MainPanelSwitcher ||
		InIndex >= MainPanelSwitcher->GetNumWidgets())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainPanelUserWidget::SwitchTargetWidget] 스위처가 nullptr이거나 입력받은 enum[%d]값이 이상합니다."),
			InIndex
		);
		return;
	}
	this->MainPanelSwitcher->SetActiveWidgetIndex(InIndex);
	this->OpenWidgetStack__.Add(MainPanelSwitcher->GetWidgetAtIndex(static_cast<int32>(InPage)));
	this->StackSize__++;
}

void UMainPanelUserWidget::CloseDetect()
{
	IOpenableWidgetInterface::Execute_CloseSelfWidget(this);
}

void UMainPanelUserWidget::BackspaceDetect()
{
	IWidgetStackHostInterface::Execute_CloseTopWidget(this);
	//CloseTopWidget_Implementation
}

void	UMainPanelUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(this->MainPanelSwitcher && this->MainPanelHome))
	{
		this->SwitchTargetWidget(EMainPanelMenuPage::Home);
		this->MainPanelHome->OnMainPanelHomeSelect.BindUFunction(this, TEXT("SwitchTargetWidget"));
		//this->OpenWidgetStack__.Add(MainPanelSwitcher->GetWidgetAtIndex(static_cast<int32>(EMainPanelMenuPage::Home)));
		//this->StackSize__++;
	}
	this->BackSpaceButton->OnClicked.AddDynamic(this, &UMainPanelUserWidget::BackspaceDetect);
	this->CloseButton->OnClicked.AddDynamic(this, &UMainPanelUserWidget::CloseDetect);
}
