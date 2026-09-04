// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "Item/MeteorItemActor.h"

#include "Engine/DataAsset.h"
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

	// 운석 액터 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	TSubclassOf<AMeteorItemActor>	MeteorActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	TObjectPtr<UItemDataAsset>		MeteorData;

	// 몇 초마다 운석 스폰을 결정할지 정하는 값 [s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSpawnTime = 0.0f;

	// 운석 스폰 사이 시간을 결정할지 정하는 값 [s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSpawnDelayTime = 0.0f;

	// 운석이 충돌할 확률 [%]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSpawnRate = 0.0f;

	// 운석이 몇 초 뒤에 충돌할지 정하는 값 [s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorAlarmTime = 0.0f;

	// 운석이 충돌할 때 우주선에 줄 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorDamage = 0.0f;

	// 운석 속도. 아마 지울 가능성이 크다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSpeed = 0.0f;

	// 운석 크기
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceMapData|Meteor")
	float	MeteorSize = 0.0f;

};
