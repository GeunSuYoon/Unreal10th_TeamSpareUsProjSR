// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemCraftDataAsset.generated.h"

class UItemDataAsset;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UItemCraftDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UItemDataAsset>	TargetItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<UItemDataAsset*, int32>	MaterialItem;

};
