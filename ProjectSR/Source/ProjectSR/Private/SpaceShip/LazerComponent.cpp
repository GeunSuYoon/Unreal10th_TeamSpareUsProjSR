// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Data/SpaceShip/LazerDataAsset.h"
#include "Item/MeteorItemActor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
ULazerComponent::ULazerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<USoundBase> DefaultLazerSFX(
		TEXT("/Game/ExtraAsset/EnergyBeam/_GenericSource/SFX/sfx_AttackUp_Cue.sfx_AttackUp_Cue"));
	LazerSFX = DefaultLazerSFX.Object;

	// ...
}

// Called when the game starts
void ULazerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OnLazerLevelChange.ExecuteIfBound(this->LazerStat__);
}


// Called every frame
void ULazerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULazerComponent::SetLazerData(ULazerDataAsset* InLazerData)
{
	//this->LazerData__ = InLazerData;
	if (InLazerData)
	{
		this->LazerStat__.Level = InLazerData->LazerStat.Level;
		this->LazerStat__.Damage = InLazerData->LazerStat.Damage;
		this->LazerStat__.ReactiveEnergy = InLazerData->LazerStat.ReactiveEnergy;
		this->LazerStat__.OperationalEnergy = InLazerData->LazerStat.OperationalEnergy;
	}
	else
	{
		this->LazerStat__.Level = 0;
		this->LazerStat__.Damage = 0.0f;
		this->LazerStat__.ReactiveEnergy = 0.0f;
		this->LazerStat__.OperationalEnergy = 0.0f;
	}
	OnLazerLevelChange.ExecuteIfBound(this->LazerStat__);
}

// 우주선에 에너지 요청해서 에너지 비율만큼 메테오에 데미지 주기.
// this->LazerPower__ * (우주선에서 가져온 에너지) / this->ReactiveEnergy__;
void ULazerComponent::AttackMeteo__(AMeteorItemActor* InMeteor)
{
	if (this->LazerStat__.Damage <= 0.0f)
	{
		return;
	}

	checkf(this->LazerStat__.ReactiveEnergy > UE_SMALL_NUMBER,
		TEXT("Lazer ReactiveEnergy must be greater than zero when Damage is configured."));

	ASpaceShipActor* SpaceShip = CastChecked<ASpaceShipActor>(GetOwner());
	const float RequestedEnergy = SpaceShip->RequestEnergy(this->LazerStat__.ReactiveEnergy);
	const float LazerDamage = this->LazerStat__.Damage * RequestedEnergy / this->LazerStat__.ReactiveEnergy;

	if (LazerDamage > 0.0f)
	{
		InMeteor->LazerDamage(LazerDamage);
		if (LazerSFX)
		{
			UGameplayStatics::PlaySound2D(this, LazerSFX);
		}
	}

}

