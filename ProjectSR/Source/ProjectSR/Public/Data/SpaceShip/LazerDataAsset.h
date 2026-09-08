// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Engine/DataAsset.h"
#include "LazerDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API ULazerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLazerStat	LazerStat;

};

