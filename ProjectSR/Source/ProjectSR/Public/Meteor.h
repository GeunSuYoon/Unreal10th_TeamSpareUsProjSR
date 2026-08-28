// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Meteor.generated.h"

USTRUCT(BlueprintType)
struct FMeteor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	StartWorldPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	WorldVelocity;		// 절대 바꾸지 않음
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	ImpactWorldTime;	// 우주선 원점 근처를 통과하는 예정 시각 [s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorRadius;		// 운석 크기 [m]
};

/**
 *
 */
class PROJECTSR_API Meteor
{
public:
	Meteor();
	~Meteor();
};