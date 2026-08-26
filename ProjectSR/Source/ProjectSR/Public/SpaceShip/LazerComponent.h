// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LazerComponent.generated.h"

class ULazorDataAsset;
class UMeteo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API ULazerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULazerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 레이저 데이터를 세팅하는 함수 다른 값도 해당 데이터의 값으로 변경된다.
	void	SetLazorData(ULazorDataAsset* InLazorData);
	// private 멤버 변수 getter 함수.
	inline float	GetLazorPower() const { return (this->LazorPower__); }
	inline float	GetUseEnergy() const { return (this->UseEnergy__); }
	inline float	GetReactiveEnergy() const { return (this->ReactiveEnergy__); }

	// 운석 날라올 때 격추하려 불러오는 함수. TODO: 다른 팀원의 구현에 따라 선언 타입 및 내부 로직 변경 필요.
	void	AttackMeteo(UMeteo* InMeteo);

protected:

private:

	TObjectPtr<ULazorDataAsset>	LazorData__ = nullptr;

	float	LazorPower__ = 0.0f;
	float	UseEnergy__ = 0.0f;
	float	ReactiveEnergy__ = 0.0f;
};
