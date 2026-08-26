// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/LazerComponent.h"
#include "DataAsset/LazorDataAsset.h"

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

void ULazerComponent::SetLazorData(ULazorDataAsset* InLazorData)
{
	this->LazorData__ = InLazorData;
	if (InLazorData)
	{
		this->LazorPower__ = InLazorData->Power;
		this->UseEnergy__ = InLazorData->UseEnergy;
		this->ReactiveEnergy__ = InLazorData->ReactiveEnergy;
	}
	else
	{
		this->LazorPower__ = 0.0f;
		this->UseEnergy__ = 0.0f;
		this->ReactiveEnergy__ = 0.0f;
	}
}

// 우주선에 에너지 요청해서 에너지 비율만큼 메테오에 데미지 주기.
// this->LazorPower__ * (우주선에서 가져온 에너지) / this->UseEnergy__;
void ULazerComponent::AttackMeteo(UMeteo* InMeteo)
{
}

