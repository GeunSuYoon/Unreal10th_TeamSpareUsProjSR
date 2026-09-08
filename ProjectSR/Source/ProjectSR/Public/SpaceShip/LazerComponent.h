// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"

#include "Components/ActorComponent.h"
#include "LazerComponent.generated.h"

class ULazerDataAsset;
class UMeteo;
class AMeteorItemActor;
class USpaceSalvageWorldSubsystem;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnLazerLevelChange, const FLazerStat&, InLazerStat);

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
	void	BindToSubsystem(USpaceSalvageWorldSubsystem* InSubsystem);

	// 레이저 데이터를 세팅하는 함수 다른 값도 해당 데이터의 값으로 변경된다.
	void	SetLazerData(ULazerDataAsset* InLazerData);

	// private 멤버 변수 getter 함수.
	inline int32	GetLevel() const { return (this->LazerStat__.Level); }
	inline float	GetLazerPower() const { return (this->LazerStat__.Damage); }
	inline float	GetReactiveEnergy() const { return (this->LazerStat__.ReactiveEnergy); }
	inline float	GetOperationalEnergy() const { return (this->LazerStat__.OperationalEnergy); }

	void	UpdateLazerLevel() { OnLazerLevelChange.ExecuteIfBound(this->LazerStat__); }

	FOnLazerLevelChange	OnLazerLevelChange;

protected:

private:
	void	AttackMeteo__(AMeteorItemActor* InMeteor);

	FLazerStat	LazerStat__;
};
