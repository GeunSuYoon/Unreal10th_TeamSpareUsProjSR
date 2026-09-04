// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMainPanelType : uint8
{
	None			= 0 UMETA(DisplayName = "None"),
	Home			= 1 UMETA(DisplayName = "Home"),
	Warehouse		= 2 UMETA(DisplayName = "Warehouse"),
	SpaceShipStatus	= 3 UMETA(DisplayName = "SpaceShipStatus"),
	SpaceShipControl= 4 UMETA(DisplayName = "SpaceShipControl"),
	SpaceShipUpgrade= 5 UMETA(DisplayName = "SpaceShipUpgrade"),
	ItemCrafting	= 6 UMETA(DisplayName = "ItemCrafting"),
};
