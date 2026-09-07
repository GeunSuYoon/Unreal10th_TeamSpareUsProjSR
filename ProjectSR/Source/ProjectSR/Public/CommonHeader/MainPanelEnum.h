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
	Select,
	SpaceShip,
	Lazer,
	MachineArm,
};

UENUM(BlueprintType)
enum class EUpgradeMenuPage : uint8
{
	Select,
	SpaceShip,
	Lazer,
	MachineArm,
};
