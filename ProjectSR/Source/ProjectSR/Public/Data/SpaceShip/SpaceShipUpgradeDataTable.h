// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpaceShipUpgradeDataTable.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct FShipUpgradeRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceShipUpgrade")
	int32	Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceShipUpgrade")
	TMap<TObjectPtr<UItemDataAsset>, int32>	NeedItem;
};

/**
 * 
 */
// SpaceShip 내부 컴포넌트들을 업그레이드할 때도 쓰는 데이터 에셋.
UCLASS()
class PROJECTSR_API USpaceShipUpgradeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceShipUpgrade")
	int32	Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpaceShipUpgrade")
	TMap<TObjectPtr<UItemDataAsset>, int32>	NeedItem;
};
