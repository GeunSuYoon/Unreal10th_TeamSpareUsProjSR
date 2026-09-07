// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainPanel/Status/SpaceShipActorStatusUserWidget.h"
#include "Widget/MainPanel/Status/SpaceShipStatusUserWidget.h"
#include "Widget/MainPanel/Status/LazerStatusUserWidget.h"
#include "Widget/MainPanel/Status/MachineArmStatusUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/MachineArmComponent.h"

void	USpaceShipActorStatusUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	if (InSpaceShip)
	{
		this->SpaceShipStatus->BindToSpaceShip(InSpaceShip);
		if (ULazerComponent* LazerComponent = InSpaceShip->GetLazerComponent())
		{
			this->LazerStatus->BindToLazerComponent(LazerComponent);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[USpaceShipActorStatusUserWidget::BindToSpaceShip] LazerComponent가 nullptr입니다.")
			);
		}
		if (UMachineArmComponent* MachineArmComponent = InSpaceShip->GetMachineArmComponent())
		{
			this->MachineArmStatus->BindToMachineArmComponent(MachineArmComponent);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[USpaceShipActorStatusUserWidget::BindToSpaceShip] MachineArmComponent가 nullptr입니다.")
			);
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceShipActorStatusUserWidget::BindToSpaceShip] SpaceShip이 nullptr입니다.")
		);
	}
}