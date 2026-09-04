// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Item/ItemActor.h"
#include "MeteorItemActor.generated.h"

/**
 * 
 */

class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
class UCameraShakeBase;

UCLASS()
class PROJECTSR_API AMeteorItemActor : public AItemActor
{
	GENERATED_BODY()
	
public:
	void	InitMeteor(const FMeteor& InMeteor, const FVector& ShipCenter, float InDespawnDistance);

	void	SetDamage(float InDamage) { this->Damage__ = InDamage; }
	void	LazerDamage(float InDamage) { this->Damage__ -= InDamage; }

	virtual void	Tick(float DeltaSeconds) override;

	void	HandleImpact();

protected:
	virtual void	NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void	OnSpawnFromPool_Implementation() override;

	virtual void	OnReturnToPool_Implementation() override;

private:
	float	Damage__ = 0.0f;
	float	DespawnDist__ = 0.0f;
	//FVector	ClosestApproachWorldPos__ = FVector::Zero();
	FVector	MoveDir__ = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, Category = "Meteor")
	TObjectPtr<UNiagaraComponent>	MoveVFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor")
	TObjectPtr<USoundBase>			MoveSFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TObjectPtr<UNiagaraSystem>		ImpactVFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TObjectPtr<USoundBase>			ImpactSFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TSubclassOf<UCameraShakeBase>	ImpactCameraShake__;

	bool	bImpactResolved__ = false;
};
