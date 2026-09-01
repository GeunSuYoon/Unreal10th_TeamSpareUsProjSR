// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "Component/EquipComponent.h"
#include "EquipmentDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FEquipmentStatModifier
{
	GENERATED_BODY()

	// 절대 수치 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
	float HealthBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
	float OxygenBonus = 0.0f;

	// 배율 보너스 - 기본값 1.0 (100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
	float OxygenDrainMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
	float MoveSpeedMultiplier = 1.0f;
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API UEquipmentDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	// 장착될 슬롯 - SpaceSuit, DragMachine, Weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;

	// --- 우주복 스탯 보너스 구조체 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Stats")
	FEquipmentStatModifier StatModifiers;
};
