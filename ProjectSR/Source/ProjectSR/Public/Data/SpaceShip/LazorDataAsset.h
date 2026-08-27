// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LazorDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API ULazorDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 운석에 줄 수 있는 데미지. 운석의 데미지에서 power를 뺀 값의 운석을 소환한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazor")
	float	Power;
	// 운석에 데미지를 줄 때 소모하는 에너지. 우주선에서 사용할 수 있는 에너지에 비례해 power를 조정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazor|Energy")
	float	ReactiveEnergy;
	// 하루마다 우주선 에너지 소모량. 따로 사용하지 않더라도 하루에 우주선이 소모하는 에너지에 이 값을 추가한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lazor|Energy")
	float	OperationalEnergy;

};
