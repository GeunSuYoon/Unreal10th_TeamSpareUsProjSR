// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Item/ItemActor.h"
#include "MeteorItemActor.generated.h"

/**
 * 
 */
class ASpaceShipActor;

class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
class UCameraShakeBase;
class UStaticMesh;

UCLASS()
class PROJECTSR_API AMeteorItemActor : public AItemActor
{
	GENERATED_BODY()
	
public:
	AMeteorItemActor();

	void	InitMeteor(const FMeteor& InMeteor, const FVector& ShipCenter, float InDespawnDistance);

	void	SetDamage(float InDamage) { this->Damage__ = InDamage; }
	void	LazerDamage(float InDamage);
	bool IsMeteorActive() const { return bMeteorActive__; }

	virtual void	Tick(float DeltaSeconds) override;

	void	HandleImpact(ASpaceShipActor* InSpaceShipActor);

protected:
	virtual void	NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void	OnSpawnFromPool_Implementation() override;

	virtual void	OnReturnToPool_Implementation() override;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

private:
	float	Damage__ = 0.0f;
	float	DespawnDist__ = 0.0f;
	//FVector	ClosestApproachWorldPos__ = FVector::Zero();
	FVector	MoveDir__ = FVector::Zero();

	UPROPERTY(EditDefaultsOnly, Category = "Meteor")
	TObjectPtr<UNiagaraComponent>	MoveVFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor")
	TObjectPtr<USoundBase>			MoveSFX__ = nullptr;

	// Visual used by physical meteors. Kept separate from the legacy item-data
	// mesh so DA_TestMeteor cannot turn the meteor back into the engine test cube.
	UPROPERTY(EditDefaultsOnly, Category = "Meteor")
	TSoftObjectPtr<UStaticMesh> MeteorVisualMesh__ = TSoftObjectPtr<UStaticMesh>(
		FSoftObjectPath(TEXT("/Game/ExtraAsset/ModularSpacePack/Meshes/SM_Asteroid1.SM_Asteroid1")));

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TObjectPtr<UNiagaraSystem>		ImpactVFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TObjectPtr<USoundBase>			ImpactSFX__ = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Meteor|Impact")
	TSubclassOf<UCameraShakeBase>	ImpactCameraShake__;

	bool	bImpactResolved__ = false;
	bool bMeteorActive__ = false;
	FVector ExitCenter__ = FVector::ZeroVector;
};
