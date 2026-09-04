// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/LazerComponent.h"
#include "Data/SpaceShip/LazerDataAsset.h"

// Sets default values for this component's properties
ULazerComponent::ULazerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void ULazerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULazerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULazerComponent::SetLazerData(ULazerDataAsset* InLazerData)
{
	this->LazerData__ = InLazerData;
	if (InLazerData)
	{
		this->LazerDamage__ = InLazerData->Damage;
		this->ReactiveEnergy__ = InLazerData->ReactiveEnergy;
		this->OperationalEnergy__ = InLazerData->OperationalEnergy;
	}
	else
	{
		this->LazerDamage__ = 0.0f;
		this->ReactiveEnergy__ = 0.0f;
		this->OperationalEnergy__ = 0.0f;
	}
}

// 우주선에 에너지 요청해서 에너지 비율만큼 메테오에 데미지 주기.
// this->LazerPower__ * (우주선에서 가져온 에너지) / this->ReactiveEnergy__;
void ULazerComponent::AttackMeteo(UMeteo* InMeteo)
{
	// 운석에 데미지 주기 (운석 자체 데미지에서 Lazer의 Damage만큼 빼기)
}

