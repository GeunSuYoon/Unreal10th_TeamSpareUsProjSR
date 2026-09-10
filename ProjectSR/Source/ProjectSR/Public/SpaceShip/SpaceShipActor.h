// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/DurabilityInterface.h"
#include "Interface/InventoryComponentInterface.h"
#include "CommonHeader/SpaceShipStruct.h"
#include "Data/SpaceShip/SpaceShipDataAsset.h"
//#include "comp"

#include "GameFramework/Actor.h"
#include "SpaceShipActor.generated.h"

class USpaceMapDataAsset;

class ULazerComponent;
class USpaceShipUpgradeComponent;
class UMachineArmComponent;
class UInventoryComponent;
class UCraftingComponent;
class UMeteorAvoidanceComponent;

class AMainPanelActor;
class ASpaceShipVisualActor;
class ADoorButtonActor;

class USphereComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSpaceShipRotate, const FRotator&, InSpaceShipRotate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSpaceShipLevelChange, const FSpaceShipStat&, InSpaceShipStat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpaceShipStatChange, float, InCurrentValue, float, InMaxValue);

UCLASS()
class PROJECTSR_API ASpaceShipActor : public AActor, public IDurabilityInterface, public IInventoryComponentInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceShipActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Disable for save loading or call GetUpgradeComponent()->InitializeLevelZero() after custom setup.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade|Initialization")
	bool bInitializeLevelZeroOnBeginPlay = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) 
		override;

	// Getter 함수
	inline int32	GetLevel() const { return (this->Level_); }
	bool IsDoorClosed() const;
	const FSpaceShipStat& GetStat() const { return SpaceShipStat_; }
	void ApplySpaceShipStat(const FSpaceShipStat& NewStat);
	UFUNCTION(BlueprintPure, Category = "Upgrade")
	inline float	GetMaxDurability() const { return (this->SpaceShipStat_.MaxDurability);	}
	inline float	GetCurrentDurability() const { return (this->CurrentDurability_); }
	inline float	GetMaxEnergy() const { return (this->SpaceShipStat_.MaxEnergy); }
	inline float	GetCurrentEnergy() const { return (this->CurrentEnergy_); }
	inline float	GetMoveSpeed() const { return (this->SpaceShipStat_.MoveSpeed); }
	inline float	GetSafeAreaRadius() const { return (this->SafeAreaRadius_); }

	inline USpaceShipUpgradeComponent*	GetUpgradeComponent() const { return (this->UpgradeComponent_); }
	inline ULazerComponent*				GetLazerComponent() const { return (this->LazerComponent_); }
	inline UMachineArmComponent*		GetMachineArmComponent() const { return (this->MachineArmComponent_); }
	inline UMeteorAvoidanceComponent*	GetMeteorAvoidance() const { return (this->MeteorAvoidanceComponent_); }
	inline UInventoryComponent*			GetWarehouse() const { return (this->WarehouseComponent_); }
	inline UCraftingComponent*			GetCraftingComponent() const { return (this->CraftingComponent_); }

	inline AMainPanelActor*				GetMainPanelActor() const { return (this->MainPanelActor_); }

	virtual UInventoryComponent*	GetInventoryComponent_Implementation() override;

	// 하루 끝날 때 불러올 함수
	void	EndOfDay();

	void	MeteorDetect(const USpaceMapDataAsset* InMapData);

	// CurrentEnergy에서 에너지를 요청하는 함수
	float	RequestEnergy(float InEnergy);

	UFUNCTION(BlueprintCallable)
	virtual void	RepairDurability_Implementation(float InDurability) override;

	UFUNCTION(BlueprintCallable)
	virtual void	ConsumDurability_Implementation(float InDurability) override;

	UFUNCTION(BlueprintCallable)
	void	GainEnergy(float InEnergy);

	UFUNCTION(BlueprintCallable)
	void	UseEnergy(float InEnergy);

	FOnSpaceShipRotate	OnSpaceShipRotate;

	UFUNCTION(BlueprintCallable)
	ADoorButtonActor*	GetDoorButtonActor() { return (this->DoorButtonActor_); }

	// 블루프린트 테스트용 함수
	UFUNCTION(BlueprintCallable)
	UMeteorAvoidanceComponent*	GetMeteorAvoidanceComponent() { return (this->MeteorAvoidanceComponent_); }

	UFUNCTION(BlueprintCallable)
	void	SpaceShipMoveInput(const FVector2D& InInput);

	void	SetSpaceShipData(USpaceShipDataAsset* InSpaceShipData);

	void	UpdateSpaceShipLevel();

	FOnSpaceShipLevelChange	OnSpaceShipLevelChange;
	FOnSpaceShipStatChange	OnDurabilityChange;
	FOnSpaceShipStatChange	OnEnergyChange;
protected:
	void	SpaceShipRotateInput_(const FVector2D& InInput);

	UFUNCTION()
	void	DetectDoorButtonClick_();
	void	UpdateDoorRotation_();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UChildActorComponent>		SpaceShipVisualActor_ = nullptr;

	// 지금 우주선의 강화 단계
	UPROPERTY(BlueprintReadOnly, Category = "Level")
	int32	Level_ = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainPanel")
	TObjectPtr<UChildActorComponent>		MainPanel_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainPanel")
	TObjectPtr<AMainPanelActor>				MainPanelActor_ = nullptr;

	// 문 여닫기 관련 변수들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	TObjectPtr<UChildActorComponent>		DoorButton_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	TObjectPtr<ADoorButtonActor>			DoorButtonActor_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	TObjectPtr<UStaticMeshComponent>		DoorMesh_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	float			DoorInteractTime_ = 0.02f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	float			DoorOpenAngle_ = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	float			DoorCloseAngle_ = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DoorButton")
	float			DoorRotationSpeed_ = 90.0f;

	FTimerHandle	DoorMoveTimerHandle_;

	FRotator		DoorTargetRotate_;

	bool			bIsDoorOpen_ = false;


	// 우주선이 가지고 있는 컴포넌트
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USpaceShipUpgradeComponent>	UpgradeComponent_ = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<ULazerComponent>				LazerComponent_ = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMachineArmComponent>		MachineArmComponent_ = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInventoryComponent>			WarehouseComponent_ = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMeteorAvoidanceComponent>	MeteorAvoidanceComponent_ = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UCraftingComponent>			CraftingComponent_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	FSpaceShipStat	SpaceShipStat_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Durability")
	float			CurrentDurability_ = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Energy")
	float			CurrentEnergy_ = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Warehouse")
	float			CurrentCapacity_ = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SafeArea")
	TObjectPtr<USphereComponent>	SafeArea_ = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SafeArea")
	float							SafeAreaRadius_ = 1500.0f;

};
