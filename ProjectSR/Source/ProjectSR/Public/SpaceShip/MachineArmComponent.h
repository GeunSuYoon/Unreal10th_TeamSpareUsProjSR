// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MachineArmComponent.generated.h"

class UMachineArmDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UMachineArmComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMachineArmComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 데이터 에셋 설정 함수
	void	SetMachineArmData(UMachineArmDataAsset* InMachineArmData);

	// Getter 함수
	inline float	GetItemCollectTime() const { return (this->ItemCollectTime__); }
	inline float	GetItemCollectWeight() const { return (this->ItemCollectWeight__); }
	inline float	GetReactiveEnergy() const { return (this->ReactiveEnergy__); }

protected:

private:

	TObjectPtr<UMachineArmDataAsset>	MachineArmData__ = nullptr;

	float	ItemCollectTime__ = 0.0f;
	float	ItemCollectWeight__ = 0.0f;
	float	ReactiveEnergy__ = 0.0f;
};
