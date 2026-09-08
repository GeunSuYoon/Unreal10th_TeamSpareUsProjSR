// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Components/ActorComponent.h"
#include "MachineArmComponent.generated.h"

class UMachineArmDataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMachineArmLevelChange, const FMachineArmStat&, InMachineArmStat);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UMachineArmComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMachineArmComponent();

	void	UpdateMachineArmLevel() { OnMachineArmLevelChange.ExecuteIfBound(this->MachineArmStat__); }

	FOnMachineArmLevelChange	OnMachineArmLevelChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 데이터 에셋 설정 함수
	void	SetMachineArmData(UMachineArmDataAsset* InMachineArmData);

	// Getter 함수
	inline int32	GetLevel() const { return (this->MachineArmStat__.Level); }
	inline float	GetItemCollectTime() const { return (this->MachineArmStat__.ItemCollectTime); }
	inline float	GetItemCollectWeight() const { return (this->MachineArmStat__.ItemCollectWeight); }
	inline float	GetOperationalEnergy() const { return (this->MachineArmStat__.OperationalEnergy); }

protected:

private:

	TObjectPtr<UMachineArmDataAsset>	MachineArmData__ = nullptr;

	FMachineArmStat	MachineArmStat__;
};
