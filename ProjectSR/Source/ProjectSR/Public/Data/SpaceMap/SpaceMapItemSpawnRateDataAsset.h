// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Item/ItemDataAsset.h"
#include "SpaceMapItemSpawnRateDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceMapItemSpawnRateDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	FText	MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	TMap<TObjectPtr<UItemDataAsset>, int32>	ItemSpawnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	float	ItemSpawnTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	float	ItemSpawnDist;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	float	ItemMoveSpeed;
};
