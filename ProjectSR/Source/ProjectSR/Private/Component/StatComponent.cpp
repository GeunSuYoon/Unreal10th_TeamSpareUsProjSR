// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StatComponent.h"
#include "Player/PlayerCharacter.h"
#include "Framework/SurvivalLoopActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}

void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializeStats__();
}

void UStatComponent::InitializeStats__()
{
	if (bStatsInitialized) return;

	OwnerCharacter = Cast<APlayerCharacter>(GetOwner());
	FEquipmentStatModifier DefaultModifier;
	DefaultModifier.HealthBonus = 0.0f;
	DefaultModifier.OxygenBonus = 0.0f;
	DefaultModifier.MoveSpeedMultiplier = 1.0f;
	DefaultModifier.OxygenDrainMultiplier = 1.0f;
	RecalculateMaxStats(DefaultModifier);

	CurrentHealth = MaxHealth;
	CurrentOxygen = MaxOxygen;
	bIsDead = false;
	bStatsInitialized = true;
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	// InitializeComponent normally ran before any actor BeginPlay. Keep this
	// idempotent fallback for unusual manually-created component lifecycles.
	InitializeStats__();

	// UI listeners are expected to bind around BeginPlay, so publish only after
	// the final current values have been assigned.
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDead || !OwnerCharacter) return;

	// --- Hunger 소모 로직 ---
	//float ActualHungerDrain = BaseHungerDrainRate;

	//if (OwnerCharacter->IsBoosting())
	//{
	//	ActualHungerDrain *= BoostHungerDrainMultiplier;	// 캐릭터 부스트 상태시 허기 감소 추가 배율 적용
	//}
	//ExecuteStatCommand({ EPlayerStatType::Hunger, -ActualHungerDrain * DeltaTime, TEXT("HungerDrain") });

	// In a survival level the actual cabin/door state wins over gravity callbacks.
	bool bConsume = bIsOxygenConsume;
	if (auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>())
	{
		if (auto* Loop = Salvage->GetSurvivalLoop(); Loop && Loop->Player == OwnerCharacter)
		{
			if (Loop->State != ESurvivalState::Playing && Loop->State != ESurvivalState::WaitingForMeteor) return;
			bConsume = !Loop->IsPlayerSafe();
		}
	}
	if (!bConsume || DeltaTime <= 0.0f) return;

	const float Drain = FMath::Max(0.0f, OxygenDrainRate);
	// Only the portion of this frame spent without oxygen causes HP damage.
	const float OxygenTime = Drain > 0.0f ? CurrentOxygen / Drain : DeltaTime;
	const float SuffocationTime = CurrentOxygen <= 0.0f
		? DeltaTime : FMath::Max(0.0f, DeltaTime - OxygenTime);
	ExecuteStatCommand({ EPlayerStatType::Oxygen, -Drain * DeltaTime, TEXT("OxygenDrain") });
	if (SuffocationTime > 0.0f)
	{
		ExecuteStatCommand({ EPlayerStatType::Health, -FMath::Max(0.0f, NoOxygenDamageRate) * SuffocationTime, TEXT("Suffocation") });
	}
}

void UStatComponent::ApplyDailyRecovery(float OxygenFillRatio, float HealthRecoveryRatio)
{
	if (bIsDead || !FMath::IsFinite(OxygenFillRatio) || !FMath::IsFinite(HealthRecoveryRatio)) return;
	const float TargetOxygen = MaxOxygen * FMath::Clamp(OxygenFillRatio, 0.0f, 1.0f);
	ModifyOxygen(FMath::Max(0.0f, TargetOxygen - CurrentOxygen));
	ModifyHealth(MaxHealth * FMath::Clamp(HealthRecoveryRatio, 0.0f, 1.0f));
}

void UStatComponent::RestoreCurrentStats(float Health, float Oxygen)
{
	CurrentHealth = FMath::Clamp(FMath::IsFinite(Health) ? Health : 0.0f, 0.0f, MaxHealth);
	CurrentOxygen = FMath::Clamp(FMath::IsFinite(Oxygen) ? Oxygen : 0.0f, 0.0f, MaxOxygen);
	bIsDead = CurrentHealth <= 0.0f;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
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

//void UStatComponent::ModifyHunger(float Amount)
//{
//	CurrentHunger = FMath::Clamp(CurrentHunger + Amount, 0.0f, MaxHunger);
//	OnHungerChanged.Broadcast(CurrentHunger, MaxHunger);
//}

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
	//case EPlayerStatType::Hunger:
	//	ModifyHunger(Command.Amount);
	//	break;
	case EPlayerStatType::Oxygen:
		ModifyOxygen(Command.Amount);
		break;
	}

	CommandHistory.Add(Command);
	if (CommandHistory.Num() > MaxHistorySize)
	{
		CommandHistory.RemoveAt(0);
	}

	//UE_LOG(LogTemp, Log, TEXT("[StatCommand] Source: %s | Type: %s | Amount: %.2f"),
	//	*Command.Source,
	//	*UEnum::GetValueAsString(Command.StatType),
	//	Command.Amount);
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

	// InitializeComponent runs before UI listeners are expected to bind. During
	// that first calculation BeginPlay publishes the finalized current values.
	if (bStatsInitialized)
	{
		OnOxygenChanged.Broadcast(CurrentOxygen, MaxOxygen);
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
}

