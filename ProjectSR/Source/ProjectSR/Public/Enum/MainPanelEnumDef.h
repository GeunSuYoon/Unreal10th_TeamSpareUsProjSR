// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMainPanelType : uint8
{
	None				UMETA(DisplayName = "None"),
	Home				UMETA(DisplayName = "Home"),
	SpaceShipStatus		UMETA(DisplayName = "SpaceShipStatus"),
	Warehouse			UMETA(DisplayName = "Warehouse"),
	SetCourse			UMETA(DisplayName = "SetCourse"),
	SpaceShipControl	UMETA(DisplayName = "SpaceShipControl"),
	SpaceShipUpgrade	UMETA(DisplayName = "SpaceShipUpgrade"),
	ItemCrafting		UMETA(DisplayName = "ItemCrafting"),
};
