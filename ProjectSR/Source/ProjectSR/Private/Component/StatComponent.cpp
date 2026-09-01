// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StatComponent.h"
#include "Player/PlayerCharacter.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	
	// 스탯 기본값으로 초기화
	RecalculateMaxStats(0.0f, 0.0f);
	CurrentHealth = MaxHealth;
	CurrentHunger = MaxHunger;
	CurrentOxygen = MaxOxygen;
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDead || !OwnerCharacter) return;

	// --- Hunger 소모 로직 ---
	float ActualHungerDrain = BaseHungerDrainRate;

	if (OwnerCharacter->IsBoosting())
	{
		ActualHungerDrain *= BoostHungerDrainMultiplier;	// 캐릭터 부스트 상태시 허기 감소 추가 배율 적용
	}
	ModifyHunger(-ActualHungerDrain * DeltaTime);

	// --- Oxygen 소모 로직 --- 무중력 상태시
	if (bIsOxygenConsume)
	{
		ModifyOxygen(-OxygenDrainRate * DeltaTime);
	}
	else
	{
		ModifyOxygen(OxygenRecoverRate * DeltaTime);
	}

	// --- 체력 패널티 --- 허기, 산소 고갈시
	if (CurrentHunger <= 0.0f)
	{
		ModifyHealth(-StarvationDamageRate * DeltaTime);
	}
	if (CurrentOxygen <= 0.0f)
	{
		ModifyHealth(-NoOxygenDamageRate * DeltaTime);
	}
}

void UStatComponent::ModifyHealth(float Amount)
{
	if (bIsDead) return;

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnPlayerDeath.Broadcast();
	}
}

void UStatComponent::ModifyHunger(float Amount)
{
	CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0.0f, MaxHunger);
	OnHungerChanged.Broadcast(CurrentHunger, MaxHunger);
}

void UStatComponent::ModifyOxygen(float Amount)
{
	CurrentOxygen = FMath::Clamp(CurrentOxygen + Amount, 0.0f, MaxOxygen);
	OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
}

// 장비 착용시 보너스 스탯값 더해서 UI 갱신
void UStatComponent::RecalculateMaxStats(float OxygenBonus, float HealthBonus)
{
	MaxOxygen = BaseMaxOxygen + OxygenBonus;
	MaxHealth = BaseMaxHealth + HealthBonus;

	// 장비 보너스 로직 확정 전까지 Base값 채워두기
	MoveSpeed = BaseMoveSpeed;
	ZeroGravityMoveSpeed = BaseZeroGravityMoveSpeed;
	BoostSpeed = BaseBoostSpeed;
	ZeroGravityBoostSpeed = BaseZeroGravityBoostSpeed;
	CrouchSpeed = BaseCrouchSpeed;

	CurrentOxygen = FMath::Min(CurrentOxygen, MaxOxygen);
	CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);

	OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

