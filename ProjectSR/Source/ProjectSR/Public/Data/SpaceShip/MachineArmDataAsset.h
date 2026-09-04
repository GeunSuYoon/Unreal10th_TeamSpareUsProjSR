// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MachineArmDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UMachineArmDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
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
