// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Item/ItemDataAsset.h"
#include "SpaceMapDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceMapDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData")
	FText	MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Item")
	TMap<TObjectPtr<UItemDataAsset>, int32>	ItemSpawnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Item")
	float	ItemSpawnTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Item")
	float	ItemSpawnDist = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Item")
	float	ItemMoveSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Item")
	int32	ItemSpawnInitCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorAlarmTimer = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSize = 0.0f;

};
