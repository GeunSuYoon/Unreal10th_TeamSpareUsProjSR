// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MeteorItemActor.h"

void AMeteorItemActor::InitMeteor(const FMeteor& InMeteor, const FVector& ShipCenter, float InDespawnDist)
{
	this->ClosestApproachWorldPos__ = ShipCenter + InMeteor.ClosestApproachPos;
	this->MoveDir__ = InMeteor.MoveDir;
	this->DespawnDist__ = InDespawnDist;
}

void AMeteorItemActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector	FromClosestPoint = GetActorLocation() - this->ClosestApproachWorldPos__;
	float	PassedDistance = FVector::DotProduct(FromClosestPoint, this->MoveDir__);

	if (PassedDistance >= this->DespawnDist__)
	{
		FinishUsingPoolable();
	}
}
