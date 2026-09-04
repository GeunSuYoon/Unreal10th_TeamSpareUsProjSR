// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Meteor.generated.h"

USTRUCT(BlueprintType)
struct FMeteor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorRemainTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSpawnLeadTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MeteorSize = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	MoveDir = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	StartPos = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	EndPos = FVector::Zero();

	void	Clear()
	{
		MeteorRemainTime = 0.0f;
		MeteorSpawnLeadTime = 0.0f;
		MeteorDamage = 0.0f;
		MeteorSpeed = 0.0f;
		MeteorSize = 0.0f;
		MoveDir = FVector::Zero();
		StartPos = FVector::Zero();
		EndPos = FVector::Zero();
	}
};

/**
 *
 */
