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
	inline int32	GetLevel() const { return (this->Level_); }
	inline float	GetItemCollectTime() const { return (this->ItemCollectTime__); }
	inline float	GetItemCollectWeight() const { return (this->ItemCollectWeight__); }
	inline float	GetOperationalEnergy() const { return (this->OperationalEnergy__); }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32	Level_ = 0;

private:

	TObjectPtr<UMachineArmDataAsset>	MachineArmData__ = nullptr;

	// 초당 아이템 획득량 [sec], 실제 보일 때는 min으로 변환 필요함
	float	ItemCollectTime__ = 0.0f;
	// 획득 가능한 아이템 무게 [kg]
	float	ItemCollectWeight__ = 0.0f;
	float	OperationalEnergy__ = 0.0f;
};
