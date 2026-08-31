// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Meteor.generated.h"

// 운석 충돌은 적당한 미니게임으로 만들자...
USTRUCT(BlueprintType)
struct FMeteor
{
	GENERATED_BODY()

	// 운석의 우주선과 상대 위치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	RelativePosition;

	// 운석이 우주선 주변에 스폰될 위치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	StartWorldPosition;

	// 운석 이동 방향, normvector
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	MoveDir;

	// 운석 이동 속력 [m/s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	MoveSpeed;

	// 운석 이동 속도, MoveDir * MoveSpeed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	FVector	WorldVelocity;

	// 우주선 원점 근처를 통과하는 예정 시각 [s]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	ImpactWorldTime;

	// 운석 크기 [m]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Meteor")
	float	Radius;

	void	SetMoveDir(const FVector& InMoveDir)
	{
		this->MoveDir = InMoveDir;
		this->WorldVelocity = this->MoveDir * this->MoveSpeed;
	}
	//inline void	SetWorldVelocity()	{}
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