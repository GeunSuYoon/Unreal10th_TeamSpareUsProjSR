// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpaceShipStruct.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpaceShipStat
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip")
	int32	Level = 0;

	// 우주선 내구도 관련 변수
	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip|Durability")
	float	MaxDurability = 0.0f;

	// 우주선 내부 에너지 관련 변수
	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip|Energy")
	float	MaxEnergy = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip|Energy")
	float	OperationalEnergy = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip|Warehouse")
	float	MaxCapacity = 0.0f;

	// 가상의 우주선 이동 속도
	UPROPERTY(BlueprintReadOnly, Category = "SpaceShip|MoveSpeed")
	float	MoveSpeed = 100.0f;

};

USTRUCT(BlueprintType)
struct FLazerStat
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Lazer")
	int32	Level = 0;

	// 운석에 줄 수 있는 데미지. 운석의 데미지에서 power를 뺀 값의 운석을 소환한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazer")
	float	Damage;
	// 운석에 데미지를 줄 때 소모하는 에너지. 우주선에서 사용할 수 있는 에너지에 비례해 power를 조정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazer|Energy")
	float	ReactiveEnergy;
	// 하루마다 우주선 에너지 소모량. 따로 사용하지 않더라도 하루에 우주선이 소모하는 에너지에 이 값을 추가한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazer|Energy")
	float	OperationalEnergy;

};

USTRUCT(BlueprintType)
struct FMachineArmStat
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "MachineArm")
	int32	Level = 0;

	// 우주 쓰레기 탐사 시 아이템을 수집하는 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MachineArm")
	float	ItemCollectTime;

	// 우주 쓰레기 탐사 시 수거 가능한 아이템의 최대 무게
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MachineArm")
	float	ItemCollectWeight;

	// 하루마다 우주선 에너지 소모량. 따로 사용하지 않더라도 하루에 우주선이 소모하는 에너지에 이 값을 추가한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MachineArm|Energy")
	float	OperationalEnergy;

};
