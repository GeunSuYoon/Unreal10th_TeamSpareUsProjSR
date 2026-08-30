// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Meteor.h"
#include "SpaceSalvageWorldSubsystem.generated.h"

class ASpaceShipActor;
class ASpaceRootActor;
class UMeteorAvoidanceComponent;
class UItemDataAsset;
class USpaceMapItemSpawnRateDataAsset;
//class AMeteorActor;

/**
 *
 */
UCLASS()
class PROJECTSR_API USpaceSalvageWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool	ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void	Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void	OnWorldBeginPlay(UWorld& InWorld) override;

	virtual void	Deinitialize() override;

	virtual void	Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(
			USpaceSalvageWorldSubsystem,
			STATGROUP_Tickables
		);
	}

	UFUNCTION(BlueprintCallable)
	void	SetSpaceMapData(USpaceMapItemSpawnRateDataAsset* InItemRateData);
	void	RegisterSpaceShipActor(ASpaceShipActor* InSpaceShip);
	void	RegisterMeteorAvoidance(UMeteorAvoidanceComponent* InAvoidanceComponent);
	//void	RotateSpaceRoot(const FVector2D& InRotationInput);
	void	RegisterVirtualMeteor(const FMeteor& InMeteor);
	void	EndOfDay();

	UFUNCTION(BlueprintCallable)
	void	SpaceShipRotateDetect(const FRotator& InRotate);
	ASpaceRootActor*	GetSpaceRootActor() { return (this->SpaceRootActor__); }
private:
	void	SpawnSpaceRoot__();
	void	SpawnItemActor__();
	void	UpdateVirtualMeteors__(float CurrentWorldTime);
	void	ResolveMeteor__(FMeteor& Meteor);

	UItemDataAsset*	SelectSpawnItemData__();
	//void	ReturnMeteor(AMeteorActor* Meteor);

	//AMeteorActor*	AcquireMeteor();

	UPROPERTY()
	TObjectPtr<ASpaceRootActor>					SpaceRootActor__ = nullptr;

	UPROPERTY()
	TObjectPtr<ASpaceShipActor>					SpaceShipActor__ = nullptr;

	TObjectPtr<UMeteorAvoidanceComponent>		MeteorAvoidanceComponent__ = nullptr;

	TObjectPtr<USpaceMapItemSpawnRateDataAsset>	ItemSpawnRateData__ = nullptr;
	// USTRUCT는 값으로 보관
	TArray<FMeteor> ActiveVirtualMeteors__;

	float	ItemSpawnDist__ = 5000.0f;
	float	ItemMoveSpeed__ = 300.0f;

	// Actor는 UObject이므로 TObjectPtr 사용 가능
	//UPROPERTY()
	//TArray<TObjectPtr<AMeteorActor>> InactiveMeteorPool_;

	FTimerHandle	ItemSpawnHandler__;
	float			ItemSpawnTimer__ = 0.0f;

};