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
	if (!Salvage || !IsValid(SpaceShip) || !IsValid(Player) || !Player->FindComponentByClass<UStatComponent>() || !bHasFirstDay || !FMath::IsFinite(DayDuration) || DayDuration <= 0.0f || (Salvage->SurvivalLoop.IsValid() && Salvage->SurvivalLoop.Get() != this))
	{
		UE_LOG(LogTemp, Error, TEXT("Survival setup invalid: assign ship, player, day-one map and positive day duration; use one loop actor."));
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
	++CurrentDay;
	int32 BestDay = 0;
	for (const auto& Entry : Maps)
	{
		if (Entry.StartDay <= CurrentDay && Entry.StartDay > BestDay) { BestDay = Entry.StartDay; CurrentMap = Entry.MapData; }
	}
	State = ESurvivalState::Playing;
	RemainingDayTime = DayDuration;
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->SetSpaceMapData(CurrentMap);
	UpdateGravity__();
	OnDayStarted.Broadcast(CurrentDay, CurrentMap);
	if (State == ESurvivalState::GameOver) { return; }
	if (CurrentDay % 3 == 0)
	{
		const float Damage = FMath::Max(0.0f, FirstSolarWindDamage) + (CurrentDay / 3 - 1) * FMath::Max(0.0f, SolarWindDamageIncrease);
		IDurabilityInterface::Execute_ConsumDurability(SpaceShip, Damage);
	}
}

void ASurvivalLoopActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver) { return; }
	if (!IsValid(Player)) { NotifyPlayerDeath(); return; }
	if (!IsValid(SpaceShip)) { EndGame__(ESurvivalEndReason::ShipDestroyed); return; }
	UpdateGravity__();
	if (State == ESurvivalState::Playing)
	{
		RemainingDayTime = FMath::Max(0.0f, RemainingDayTime - DeltaSeconds);
		if (RemainingDayTime <= 0.0f) { FinishDay(); }
	}
	else if (!GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->HasPendingMeteor()) { BeginDay__(); }
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
		const bool bGravity = IsPlayerInside() && SpaceShip->IsDoorClosed();
		if (bGravity && Movement->GetGravityState() != EGravityState::GravityMode) { Movement->ExitZeroGravity(); }
		if (!bGravity && Movement->GetGravityState() != EGravityState::ZeroGravityMode) { Movement->EnterZeroGravity(); }
	}
}

void ASurvivalLoopActor::NotifyMeteorImpact(ASpaceShipActor* HitShip)
{
	if (HitShip != SpaceShip || State == ESurvivalState::Ready || State == ESurvivalState::GameOver || IsPlayerInside()) { return; }
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
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameOver.Broadcast(Reason, CurrentDay);
	UGameplayStatics::SetGamePaused(this, true);
}

void ASurvivalLoopActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
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
