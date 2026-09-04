// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilFunction.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTSR_API FUtilFunction
{
	GENERATED_BODY()

public:
	//UtilFunctionClass();
	//~UtilFunctionClass();
	// InLinePoint를 InLineDirectine 기울기로 지나는 직선과 InPoint 사이의 최소 길이의 제곱을 반환하는 함수
	static float	GetPointToLineDistanceSquared(const FVector& InPoint, const FVector& InLinePoint, const FVector& InLineDirection);
};
