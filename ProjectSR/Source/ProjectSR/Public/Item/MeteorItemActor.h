// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void	SetDamage(float InDamage) { this->Damage__ = InDamage; }
	void	LazerDamage(float InDamage) { this->Damage__ -= InDamage; }

private:
	float	Damage__ = 0.0f;
};
