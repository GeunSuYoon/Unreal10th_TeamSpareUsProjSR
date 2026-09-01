// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Item/ItemActor.h"
#include "MeteorItemActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API AMeteorItemActor : public AItemActor
{
	GENERATED_BODY()
	
public:
	void	InitMeteor(
		const FMeteor& InMeteor,
		const FVector& ShipCenter,
		float InDespawnDistance
	);

	void	SetDamage(float InDamage) { this->Damage__ = InDamage; }
	void	LazerDamage(float InDamage) { this->Damage__ -= InDamage; }

	virtual void Tick(float DeltaSeconds) override;

private:
	float	Damage__ = 0.0f;
	float	DespawnDist__ = 0.0f;
	FVector	ClosestApproachWorldPos__ = FVector::Zero();
	FVector	MoveDir__ = FVector::Zero();

};
