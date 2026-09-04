// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemActor.h"
#include "CommonHeader/Meteor.h"

#include "Subsystems/WorldSubsystem.h"
#include "SpaceSalvageWorldSubsystem.generated.h"
//#include "Components/SphereComponent.h"

class ASpaceShipActor;
class ASpaceRootActor;
class UMeteorAvoidanceComponent;
class UItemDataAsset;
class USpaceMapDataAsset;
class USphereComponent;
class AMeteorItemActor;
//class AItemActor;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSpaceMapUpdate, const float, InDist);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMeteorSpawn, AMeteorItemActor*, InMeteor);

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

	// 블루프린트 테스트용 함수
	UFUNCTION(BlueprintCallable)
	void	SetSafeArea(float InArea);

	UFUNCTION(BlueprintCallable)
	void	SetSpaceMapData(USpaceMapDataAsset* InSpaceMapData);

	void	RegisterSpaceShipActor(ASpaceShipActor* InSpaceShip);
	void	RegisterMeteorAvoidance(UMeteorAvoidanceComponent* InAvoidanceComponent);
	void	MeteorDetect();
	void	EndOfDay();

	UFUNCTION(BlueprintCallable)
	void	SpaceShipRotateDetect(const FRotator& InRotate);

	UFUNCTION(BlueprintCallable)
	ASpaceRootActor*	GetSpaceRootActor() { return (this->SpaceRootActor__); }

	UFUNCTION(BlueprintCallable)
	ASpaceShipActor*	GetSpaceShipActor() const { return (this->SpaceShipActor__); }

	float	GetItemSpawnDist() const { return (this->ItemSpawnDist__); }

	FOnSpaceMapUpdate	OnSpaceMapUpdate;
	FOnMeteorSpawn		OnMeteorSpawn;

protected:
	// 테스트용 코드에용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Area")
	TObjectPtr<USphereComponent> SafeAreaVisualizer_;

private:
	UFUNCTION()
	void	SpawnMeteor__(const FMeteor& InMeteor);

	void	TryStartItemSpawn__();
	void	SpawnSpaceRoot__();
	void	SpawnItemLevelStart__(int32 InitItemCount);
	void	SpawnItemActor__();
	void	SpawnItemActor__(FVector InLocation);
	void	DespawnItemActor__();

	UItemDataAsset*	SelectSpawnItemData__();

	UPROPERTY()
	TObjectPtr<ASpaceRootActor>		SpaceRootActor__ = nullptr;

	UPROPERTY()
	TObjectPtr<ASpaceShipActor>		SpaceShipActor__ = nullptr;

	//TObjectPtr<UMeteorAvoidanceComponent>		MeteorAvoidanceComponent__ = nullptr;

	TObjectPtr<USpaceMapDataAsset>	SpaceMapData__ = nullptr;
	// USTRUCT는 값으로 보관
	//TArray<FMeteor> ActiveVirtualMeteors__;

	TArray<TWeakObjectPtr<AItemActor>>	SpawnedItem__;

	float	ItemSpawnDist__ = 0.0f;
	float	ItemDespawnDist__ = 0.0f;
	float	ItemMoveSpeed__ = 0.0f;

	int32	ItemSpawnMaxRetryCount__ = 30;
	// Actor는 UObject이므로 TObjectPtr 사용 가능
	//UPROPERTY()
	//TArray<TObjectPtr<AMeteorActor>> InactiveMeteorPool_;

	FTimerHandle	ItemSpawnHandler__;
	float			ItemSpawnTimer__ = 0.0f;

	FTimerHandle	ItemDespawnHandler__;
	float			ItemDespawnTimer__ = 0.5f;

	// 테스트용 할당값
	float	SafeArea__ = 500.0f;
	float	SafeAreaSquared__ = FMath::Square(500.0f);

	float	LevelTime__ = 0.0f;
	float	MeteorSpawnTime__ = 0.0f;
	float	MeteorSpawnDelayTime__ = 0.0f;
	FTimerHandle	MeteorSpawnHandler__;
};