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

	// 장착물이 없는 순수 맨몸 상태 초기화 (보너스 0, 배율 1.0)
	FEquipmentStatModifier DefaultModifier;
	DefaultModifier.HealthBonus = 0.0f;
	DefaultModifier.OxygenBonus = 0.0f;
	DefaultModifier.MoveSpeedMultiplier = 1.0f;
	DefaultModifier.OxygenDrainMultiplier = 1.0f;
	
	// 스탯 기본값으로 초기화
	RecalculateMaxStats(DefaultModifier);

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
	ExecuteStatCommand({ EPlayerStatType::Hunger, -ActualHungerDrain * DeltaTime, TEXT("HungerDrain") });

	// --- Oxygen 소모 로직 --- 무중력 상태시
	if (bIsOxygenConsume)
	{
		ExecuteStatCommand({ EPlayerStatType::Oxygen, -OxygenDrainRate * DeltaTime, TEXT("ZeroGravityOxygenDrain") });
	}
	else
	{
		ExecuteStatCommand({ EPlayerStatType::Oxygen, OxygenRecoverRate * DeltaTime, TEXT("OxygenRecover") });
	}

	// --- 체력 패널티 --- 허기, 산소 고갈시
	if (CurrentHunger <= 0.0f)
	{
		ExecuteStatCommand({ EPlayerStatType::Health, -StarvationDamageRate * DeltaTime, TEXT("Starvation") });
	}
	if (CurrentOxygen <= 0.0f)
	{
		ExecuteStatCommand({ EPlayerStatType::Health, -NoOxygenDamageRate * DeltaTime, TEXT("Suffocation") });
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

// 커맨드 함수
void UStatComponent::ExecuteStatCommand(const FStatChangeCommand& Command)
{
	switch (Command.StatType)
	{
	case EPlayerStatType::Health:
		ModifyHealth(Command.Amount);
		break;
	case EPlayerStatType::Hunger:
		ModifyHunger(Command.Amount);
		break;
	case EPlayerStatType::Oxygen:
		ModifyOxygen(Command.Amount);
		break;
	}

	CommandHistory.Add(Command);
	if (CommandHistory.Num() > MaxHistorySize)
	{
		CommandHistory.RemoveAt(0);
	}

	UE_LOG(LogTemp, Log, TEXT("[StatCommand] Source: %s | Type: %s | Amount: %.2f"),
		*Command.Source,
		*UEnum::GetValueAsString(Command.StatType),
		Command.Amount);
}

// 장비 착용시 보너스 스탯값 더해서 UI 갱신
void UStatComponent::RecalculateMaxStats(const FEquipmentStatModifier& Modifiers)
{
	// 1. 최대 체력 및 산소 재계산 (Base + Bonus)
	MaxHealth = BaseMaxHealth + Modifiers.HealthBonus;
	MaxOxygen = BaseMaxOxygen + Modifiers.OxygenBonus;

	// 2. 이동 속도 배율 적용 (Base * Multiplier)
	MoveSpeed = BaseMoveSpeed * Modifiers.MoveSpeedMultiplier;
	ZeroGravityMoveSpeed = BaseZeroGravityMoveSpeed * Modifiers.MoveSpeedMultiplier;
	BoostSpeed = BaseBoostSpeed * Modifiers.MoveSpeedMultiplier;
	ZeroGravityBoostSpeed = BaseZeroGravityBoostSpeed * Modifiers.MoveSpeedMultiplier;
	CrouchSpeed = BaseCrouchSpeed * Modifiers.MoveSpeedMultiplier;

	// 3. 현재 수치가 최대치를 넘지 않도록 Clamp
	CurrentOxygen = FMath::Min(CurrentOxygen, MaxOxygen);
	CurrentHealth = FMath::Min(CurrentHealth, MaxHealth);

	// 4. UI 및 방송 알림
	OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

