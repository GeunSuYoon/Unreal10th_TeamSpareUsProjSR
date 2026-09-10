#include "Framework/SurvivalLoopActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Player/PlayerCharacter.h"
#include "Component/StatComponent.h"
#include "Component/InSpaceMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Data/SpaceMap/SpaceMapDataAsset.h"
#include "GameFramework/PlayerController.h"

ASurvivalLoopActor::ASurvivalLoopActor()
{
	PrimaryActorTick.bCanEverTick = true;
	InteriorBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBounds"));
	SetRootComponent(InteriorBounds);
	InteriorBounds->SetBoxExtent(FVector(500.0f, 300.0f, 200.0f));
	InteriorBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASurvivalLoopActor::BeginPlay()
{
	Super::BeginPlay();
	// All placed actors and the possessed pawn must finish BeginPlay first.
	if (bAutoStart)
	{
		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]() { StartSurvival(); }));
	}
}

bool ASurvivalLoopActor::StartSurvival()
{
	if (State != ESurvivalState::Ready) { return false; }
	auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (!SpaceShip && Salvage) { SpaceShip = Salvage->GetSpaceShipActor(); }
	if (!Player) { Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)); }
	bool bHasFirstDay = false;
	TSet<int32> Days;
	for (const auto& Entry : Maps)
	{
		if (!IsValid(Entry.MapData) || Entry.StartDay < 1 || Days.Contains(Entry.StartDay)) { return false; }
		Days.Add(Entry.StartDay);
		bHasFirstDay |= Entry.StartDay == 1;
	}
	const auto* Stats = IsValid(Player) ? Player->FindComponentByClass<UStatComponent>() : nullptr;
	if (!Salvage || !IsValid(SpaceShip) || !Stats || !bHasFirstDay
		|| !FMath::IsFinite(Stats->GetMaxOxygen()) || Stats->GetMaxOxygen() <= 0.0f
		|| !FMath::IsFinite(Stats->GetOxygenDrainRate()) || Stats->GetOxygenDrainRate() <= 0.0f
		|| !FMath::IsFinite(Stats->GetMaxOxygen() / Stats->GetOxygenDrainRate())
		|| !FMath::IsFinite(LowEnergyOxygenRatio) || !FMath::IsFinite(DailyHealthRecoveryRatio)
		|| !FMath::IsFinite(DayPreparationTimeout) || DayPreparationTimeout <= 0.0f
		|| !FMath::IsFinite(MinimumInitialSpawnRatio)
		|| (Salvage->SurvivalLoop.IsValid() && Salvage->SurvivalLoop.Get() != this))
	{
		UE_LOG(LogTemp, Error, TEXT("Survival setup invalid: assign ship, player, day-one map and positive finite oxygen capacity/drain; use one loop actor."));
		return false;
	}
	Salvage->SurvivalLoop = this;
	SpaceShip->OnDurabilityChange.AddUniqueDynamic(this, &ASurvivalLoopActor::HandleDurability__);
	Player->FindComponentByClass<UStatComponent>()->OnPlayerDeath.AddUniqueDynamic(this, &ASurvivalLoopActor::NotifyPlayerDeath);
	AttachToActor(SpaceShip, FAttachmentTransformRules::KeepWorldTransform);
	State = ESurvivalState::Playing;
	if (Player->FindComponentByClass<UStatComponent>()->GetHealth() <= 0.0f) { NotifyPlayerDeath(); return true; }
	if (SpaceShip->GetCurrentDurability() <= 0.0f) { EndGame__(ESurvivalEndReason::ShipDestroyed); return true; }
	BeginDay__();
	return true;
}

void ASurvivalLoopActor::BeginDay__()
{
	State = ESurvivalState::PreparingDay;
	RemainingDayTime = 0.0f;
	DayPreparationError.Reset();
	SetPreparationInputLocked__(true);
	int32 BestDay = 0;
	CurrentMap = nullptr;
	for (const auto& Entry : Maps)
	{
		if (Entry.StartDay <= CurrentDay + 1 && Entry.StartDay > BestDay) { BestDay = Entry.StartDay; CurrentMap = Entry.MapData; }
	}
	OnDayPreparationStarted.Broadcast(CurrentDay + 1, CurrentMap);
	if (State != ESurvivalState::PreparingDay) return;
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->PrepareDay(CurrentMap, DayPreparationTimeout, MinimumInitialSpawnRatio);
}

void ASurvivalLoopActor::CompleteDayPreparation__()
{
	auto* Stats = Player->FindComponentByClass<UStatComponent>();
	const float Capacity = Stats->GetMaxOxygen();
	const float Drain = Stats->GetOxygenDrainRate();
	// Invalid runtime equipment/config must not create an infinite or zero-length day.
	if (!FMath::IsFinite(Capacity) || Capacity <= 0.0f || !FMath::IsFinite(Drain) || Drain <= 0.0f
		|| !FMath::IsFinite(Capacity / Drain))
	{
		FailDayPreparation__(TEXT("Oxygen capacity/drain must be positive and finite."));
		return;
	}
	auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (!Salvage->ActivatePreparedDay()) { FailDayPreparation__(Salvage->GetDayPreparationError()); return; }
	++CurrentDay;
	DayDuration = Capacity / Drain;
	if (CurrentDay > 1) { ApplyDailySettlement__(); }
	if (State == ESurvivalState::GameOver) return;
	State = ESurvivalState::Playing;
	RemainingDayTime = DayDuration;
	UpdateGravity__();
	if (CurrentDay % 3 == 0)
	{
		const float Damage = FMath::Max(0.0f, FirstSolarWindDamage) + (CurrentDay / 3 - 1) * FMath::Max(0.0f, SolarWindDamageIncrease);
		IDurabilityInterface::Execute_ConsumDurability(SpaceShip, Damage);
	}
	if (State != ESurvivalState::GameOver)
	{
		SetPreparationInputLocked__(false);
		OnDayStarted.Broadcast(CurrentDay, CurrentMap);
	}
}

void ASurvivalLoopActor::FailDayPreparation__(const FString& Reason)
{
	DayPreparationError = Reason;
	State = ESurvivalState::PreparationFailed;
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->CancelDayPreparation();
	OnDayPreparationFailed.Broadcast(CurrentDay + 1, DayPreparationError);
}

bool ASurvivalLoopActor::RetryDayPreparation()
{
	if (State != ESurvivalState::PreparationFailed || !IsValid(Player) || !IsValid(SpaceShip)) return false;
	BeginDay__();
	return true;
}

float ASurvivalLoopActor::GetDayPreparationProgress() const
{
	if (State == ESurvivalState::Playing) return 1.0f;
	return GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->GetDayPreparationProgress();
}

void ASurvivalLoopActor::SetPreparationInputLocked__(bool bLocked)
{
	if (bPreparationInputLocked__ == bLocked) return;
	bPreparationInputLocked__ = bLocked;
	if (bLocked && IsValid(Player))
	{
		LockedController__ = Cast<APlayerController>(Player->GetController());
		bPlayerInputWasEnabled__ = Player->InputEnabled();
		Player->DisableInput(LockedController__.Get());
		if (auto* PC = LockedController__.Get()) { PC->SetIgnoreMoveInput(true); PC->SetIgnoreLookInput(true); }
		if (auto* Movement = Player->GetInSpaceMovementComponent())
		{
			bMovementTickWasEnabled__ = Movement->IsComponentTickEnabled();
			Movement->SetComponentTickEnabled(false);
		}
	}
	else
	{
		if (auto* PC = LockedController__.Get()) { PC->SetIgnoreMoveInput(false); PC->SetIgnoreLookInput(false); }
		if (IsValid(Player))
		{
			if (bPlayerInputWasEnabled__) Player->EnableInput(LockedController__.Get());
			if (auto* Movement = Player->GetInSpaceMovementComponent()) Movement->SetComponentTickEnabled(bMovementTickWasEnabled__);
		}
		LockedController__.Reset();
	}
}

void ASurvivalLoopActor::ApplyDailySettlement__()
{
	auto* Stats = Player->FindComponentByClass<UStatComponent>();
	if (!Stats || Stats->GetHealth() <= 0.0f) { NotifyPlayerDeath(); return; }
	const float Cost = FMath::Max(0.0f, SpaceShip->GetStat().OperationalEnergy);
	bLastDailyEnergySufficient = SpaceShip->GetCurrentEnergy() >= Cost;
	SpaceShip->UseEnergy(Cost);
	if (State == ESurvivalState::GameOver) return;
	Stats->ApplyDailyRecovery(bLastDailyEnergySufficient ? 1.0f : LowEnergyOxygenRatio, DailyHealthRecoveryRatio);
}

void ASurvivalLoopActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver) { return; }
	if (!IsValid(Player)) { NotifyPlayerDeath(); return; }
	if (!IsValid(SpaceShip)) { EndGame__(ESurvivalEndReason::ShipDestroyed); return; }
	if (State == ESurvivalState::PreparationFailed) return;
	if (State == ESurvivalState::PreparingDay)
	{
		auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
		if (Salvage->GetDayPreparationStatus() == EDayPreparationStatus::Ready) CompleteDayPreparation__();
		else if (Salvage->GetDayPreparationStatus() == EDayPreparationStatus::Failed) FailDayPreparation__(Salvage->GetDayPreparationError());
		return;
	}
	UpdateGravity__();
	if (State == ESurvivalState::Playing)
	{
		RemainingDayTime = FMath::Max(0.0f, RemainingDayTime - DeltaSeconds);
		if (RemainingDayTime <= 0.0f) { FinishDay(); }
	}
	else if (State == ESurvivalState::WaitingForMeteor && !GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->HasPendingMeteor()) { BeginDay__(); }
}

void ASurvivalLoopActor::FinishDay()
{
	if (State != ESurvivalState::Playing) { return; }
	RemainingDayTime = 0.0f;
	State = ESurvivalState::WaitingForMeteor;
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->EndOfDay();
}

bool ASurvivalLoopActor::IsPlayerInside() const
{
	if (!IsValid(Player)) { return false; }
	const FVector Local = InteriorBounds->GetComponentTransform().InverseTransformPosition(Player->GetActorLocation());
	const FVector Extent = InteriorBounds->GetUnscaledBoxExtent();
	return FMath::Abs(Local.X) <= Extent.X && FMath::Abs(Local.Y) <= Extent.Y && FMath::Abs(Local.Z) <= Extent.Z;
}

void ASurvivalLoopActor::UpdateGravity__()
{
	if (auto* Movement = Player->GetInSpaceMovementComponent())
	{
		const bool bGravity = IsPlayerSafe();
		if (bGravity && Movement->GetGravityState() != EGravityState::GravityMode) { Movement->ExitZeroGravity(); }
		if (!bGravity && Movement->GetGravityState() != EGravityState::ZeroGravityMode) { Movement->EnterZeroGravity(); }
	}
}

bool ASurvivalLoopActor::IsPlayerSafe() const
{
	return IsValid(SpaceShip) && IsPlayerInside() && SpaceShip->IsDoorClosed();
}

void ASurvivalLoopActor::NotifyMeteorImpact(ASpaceShipActor* HitShip)
{
	if (HitShip != SpaceShip || (State != ESurvivalState::Playing && State != ESurvivalState::WaitingForMeteor) || IsPlayerSafe()) { return; }
	if (IsValid(Player))
	{
		if (auto* Stats = Player->FindComponentByClass<UStatComponent>()) { Stats->ModifyHealth(-Stats->GetHealth()); }
	}
	NotifyPlayerDeath();
}

void ASurvivalLoopActor::NotifyPlayerDeath() { EndGame__(ESurvivalEndReason::PlayerDied); }
void ASurvivalLoopActor::HandleDurability__(float Current, float Maximum) { if (Current <= 0.0f) { EndGame__(ESurvivalEndReason::ShipDestroyed); } }

void ASurvivalLoopActor::EndGame__(ESurvivalEndReason Reason)
{
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver) { return; }
	State = ESurvivalState::GameOver;
	SetPreparationInputLocked__(false);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameOver.Broadcast(Reason, CurrentDay);
	UGameplayStatics::SetGamePaused(this, true);
}

void ASurvivalLoopActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SetPreparationInputLocked__(false);
	if (IsValid(SpaceShip)) { SpaceShip->OnDurabilityChange.RemoveDynamic(this, &ASurvivalLoopActor::HandleDurability__); }
	if (IsValid(Player))
	{
		if (auto* Stats = Player->FindComponentByClass<UStatComponent>()) { Stats->OnPlayerDeath.RemoveDynamic(this, &ASurvivalLoopActor::NotifyPlayerDeath); }
	}
	if (auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>())
	{
		if (Salvage->SurvivalLoop.Get() == this) { Salvage->StopSurvival(); Salvage->SurvivalLoop.Reset(); }
	}
	Super::EndPlay(EndPlayReason);
}
