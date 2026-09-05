// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainUserWidget.h"
#include "Widget/MeteorWarningUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Widget/InventoryWindowWidget.h"
#include "Interface/InventoryComponentInterface.h"
#include "Player/PlayerCharacter.h"

void UMainUserWidget::BindToPlayer(APlayerCharacter* InPlayerCharacter)
{
    if (!InPlayerCharacter)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("[UMainUserWidget::BindToStorage] InPlayerCharacter가 nullptr입니다.")
        );
        return;
    }
    this->InventoryWindow->BindToInventoryComponent(IInventoryComponentInterface::Execute_GetInventoryComponent(InPlayerCharacter));
    InPlayerCharacter->OnToggleInventory.BindUFunction(InventoryWindow, TEXT("ToggleInventoryWidget"));
}

void UMainUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShipActor)
{
	if (!InSpaceShipActor)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[UMainUserWidget::BindToSpaceShip] InSpaceShipActor가 nullptr입니다.")
		);
		return;
	}
	this->MeteoWarningWidget->SetVisibility(ESlateVisibility::Collapsed);
	this->MeteoWarningWidget->BindToMeteorAvoidanceComponent(InSpaceShipActor->GetMeteorAvoidance());
}

void UMainUserWidget::BindToCharacter(ACharacter* InCharacter)
{
}
