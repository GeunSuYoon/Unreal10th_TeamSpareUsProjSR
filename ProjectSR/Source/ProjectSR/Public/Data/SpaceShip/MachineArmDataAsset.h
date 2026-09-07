// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMachineArmStat	MachineArmStat;

};
