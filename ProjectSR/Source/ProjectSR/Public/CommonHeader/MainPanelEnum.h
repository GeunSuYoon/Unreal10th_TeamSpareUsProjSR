// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainPanelEnum.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMainPanelMenuPage : uint8
{
	Home,
	SpaceShipStatus,
	Warehouse,
	MeteorEvent,
	Upgrade,
	ItemCraft,
};

UENUM(BlueprintType)
enum class EUpgradeMenuPage : uint8
{
	Home,
	SpaceShip,
	Lazer,
	MachineArm,
};
