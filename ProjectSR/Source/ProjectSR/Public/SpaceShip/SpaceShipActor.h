// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DurabilityInterface.h"
#include "Interface/InventoryComponentInterface.h"
#include "SpaceShipActor.generated.h"

class AMainPanelActor;
class ULazerComponent;
class UMachineArmComponent;
class UInventoryComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSpaceShipRotate, const FRotator&, InSpaceShipRotate);

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter 함수
	inline int32	GetLevel() const { return (this->Level_); }
	inline float	GetMaxDurability() const { return (this->MaxDurability_); }
	inline float	GetCurrentDurability() const { return (this->CurrentDurability_); }
	inline float	GetMaxEnergy() const { return (this->MaxEnergy_); }
	inline float	GetCurrentEnergy() const { return (this->CurrentEnergy_); }
	inline float	GetMoveSpeed() const { return (this->MoveSpeed_); }
	inline float	GetSafeArea() const { return (this->SafeArea_); }
	//inline UInventoryComponent*	GetWarehouse() const { return (this->Warehouse_); }

	virtual UInventoryComponent*	GetInventoryComponent_Implementation() override;

	// 하루 끝날 때 불러올 함수
	void	EndOfDay();

	// CurrentEnergy에서 에너지를 요청하는 함수
	float	RequestEnergy(float InEnergy);

	virtual void	RepairDurability_Implementation(float InDurability) override;
	virtual void	ConsumDurability_Implementation(float InDurability) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent>	SpaceShipMesh = nullptr;

	FOnSpaceShipRotate	OnSpaceShipRotate;

protected:
	void	SpaceShipRotateInput_(const FVector2D& InInput);

	// 지금 우주선의 강화 단계
	UPROPERTY(BlueprintReadOnly, Category = "Level")
	int32	Level_ = 0;

	// 우주선이 가지고 있는 컴포넌트
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<AMainPanelActor>			MainPanel_;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<ULazerComponent>			Lazor_;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<UMachineArmComponent>	MainArm_;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInventoryComponent>		Warehouse_;

	// 우주선 내구도 관련 변수
	UPROPERTY(BlueprintReadOnly, Category = "Durability")
	float	MaxDurability_ = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Durability")
	float	CurrentDurability_ = 0.0f;

	// 우주선 내부 에너지 관련 변수
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	float	MaxEnergy_ = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	float	CurrentEnergy_ = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	float	OperationalEnergy_ = 0.0f;

	// 가상의 우주선 이동 속도
	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	float	MoveSpeed_ = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Rotate")
	float	RotateSpeed_ = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Rotate")
	FRotator	SpaceShipRotateState_;

	UPROPERTY(BlueprintReadOnly, Category = "Area")
	float	SafeArea_ = 100.0f;

};
