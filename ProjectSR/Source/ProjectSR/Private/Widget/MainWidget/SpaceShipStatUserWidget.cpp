// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainWidget/SpaceShipStatUserWidget.h"
#include "Widget/MainWidget/SpaceShipStat/SpaceShipEnergyUserWidget.h"
#include "Widget/MainWidget/SpaceShipStat/SpaceShipDurabilityUserWidget.h"

void	USpaceShipStatUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	this->SpaceShipEnergy->BindToSpaceShip(InSpaceShip);
	this->SpaceShipDurability->BindToSpaceShip(InSpaceShip);
}
