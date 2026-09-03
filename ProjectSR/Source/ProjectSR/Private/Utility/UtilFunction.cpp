// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/UtilFunction.h"
//#include "UtilFunction.h"

float FUtilFunction::GetPointToLineDistanceSquared(const FVector& InPoint, const FVector& InLinePoint, const FVector& InLineDirection)
{
	// 들어온 방향 벡터의 크기 제곱
	float	DirectionSizeSquared = InLineDirection.SizeSquared();
	if (DirectionSizeSquared <= UE_SMALL_NUMBER)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[FUtilFunction::GetPointToLineDistanceSquared] InLineDirection의 크기가 너무 작습니다.")
		);
		return (FVector::DistSquared(InPoint, InLinePoint));
	}
	// InLinePoint에서 InPoint로 가는 벡터
	FVector	LinePointToPointVector = InPoint - InLinePoint;
	// 내적 값을 선분 방향 크기 제곱으로 InLinePoint에서 InLinePoint를 포함한 InLineVector와 InPoint의 가장 가까운 점까지 거리를 구함
	float	ProjectionValue = FVector::DotProduct(LinePointToPointVector, InLineDirection) / DirectionSizeSquared;
	// 가장 가까운 점의 좌표를 구함
	FVector	PerpendicularOffsetVector = LinePointToPointVector - InLineDirection * ProjectionValue;

	// 가장 가까운 거리의 제곱을 반환
	return (PerpendicularOffsetVector.SizeSquared());
}
