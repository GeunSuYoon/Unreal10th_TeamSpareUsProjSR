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
#include "Save/SurvivalSaveSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"

ASurvivalLoopActor::ASurvivalLoopActor()
{
	PrimaryActorTick.bCanEverTick = true;
	InteriorBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBounds"));
	SetRootComponent(InteriorBounds);
	InteriorBounds->SetBoxExtent(FVector(500.0f, 300.0f, 200.0f));
	InteriorBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverClassFinder(
		TEXT("/Game/Blueprint/GeunSuYoon/Widget/MainPanel/WBP_GameOver"));
	if (GameOverClassFinder.Succeeded())
	{
		GameOverWidgetClass = GameOverClassFinder.Class;
	}
}

void ASurvivalLoopActor::BeginPlay()
{
	Super::BeginPlay();
	auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (!Salvage)
	{
		UE_LOG(LogTemp, Error, TEXT("SurvivalLoop could not find SpaceSalvageWorldSubsystem."));
		return;
	}
	// Register even when auto start is off so manual StartSurvival owns the same subsystem state.
	Salvage->RegisterSurvivalLoopActor(this);
}

bool ASurvivalLoopActor::StartSurvival()
{
	if (State != ESurvivalState::Ready) { return false; }
	auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	// StartSurvival is also a public/manual entry point and can be called before
	// this actor's BeginPlay in tests or by a spawner. Ensure subsystem ownership
	// does not depend on the auto-start registration path.
	if (Salvage && !Salvage->GetSurvivalLoop())
	{
		Salvage->RegisterSurvivalLoopActor(this);
	}
	if (!SpaceShip && Salvage) { SpaceShip = Salvage->GetSpaceShipActor(); }
	if (!Player) { Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)); }
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>();
			SaveSubsystem && !SaveSubsystem->TryApplyRequestedLoad(this))
		{
			UE_LOG(LogTemp, Error, TEXT("Survival start aborted because the requested checkpoint could not be restored."));
			return false;
		}
	}
	bool bHasFirstDay = false;
	TSet<int32> Days;
	for (const auto& Entry : Maps)
	{
		if (!IsValid(Entry.MapData) || Entry.StartDay < 1 || Days.Contains(Entry.StartDay))
		{
			UE_LOG(LogTemp, Error, TEXT("Survival setup invalid: every map must be valid and use a unique StartDay >= 1."));
			return false;
		}
		Days.Add(Entry.StartDay);
		bHasFirstDay |= Entry.StartDay == 1;
	}
	const auto* Stats = IsValid(Player) ? Player->FindComponentByClass<UStatComponent>() : nullptr;
	if (!Salvage || !IsValid(SpaceShip) || !Stats || !Stats->IsStatsInitialized() || !bHasFirstDay
		|| !FMath::IsFinite(Stats->GetMaxOxygen()) || Stats->GetMaxOxygen() <= 0.0f
		|| !FMath::IsFinite(Stats->GetOxygenDrainRate()) || Stats->GetOxygenDrainRate() <= 0.0f
		|| !FMath::IsFinite(Stats->GetMaxOxygen() / Stats->GetOxygenDrainRate())
		|| !FMath::IsFinite(DayDuration) || DayDuration <= 0.0f
		|| !FMath::IsFinite(LowEnergyOxygenRatio) || !FMath::IsFinite(DailyHealthRecoveryRatio)
		|| !FMath::IsFinite(DayPreparationTimeout) || DayPreparationTimeout <= 0.0f
		|| !FMath::IsFinite(MinimumInitialSpawnRatio)
		|| (Salvage->GetSurvivalLoop() && Salvage->GetSurvivalLoop() != this))
	{
		UE_LOG(LogTemp, Error, TEXT("Survival setup invalid: player stats must be initialized; assign ship, player, day-one map and positive finite day duration and oxygen capacity/drain; use one loop actor."));
		return false;
	}
	SpaceShip->OnDurabilityChange.AddUniqueDynamic(this, &ASurvivalLoopActor::HandleDurability__);
	Player->FindComponentByClass<UStatComponent>()->OnPlayerDeath.AddUniqueDynamic(this, &ASurvivalLoopActor::NotifyPlayerDeath);
	AttachToActor(SpaceShip, FAttachmentTransformRules::KeepWorldTransform);
	State = ESurvivalState::Playing;
	if (Player->FindComponentByClass<UStatComponent>()->GetHealth() <= 0.0f) { NotifyPlayerDeath(); return true; }
	if (SpaceShip->GetCurrentDurability() <= 0.0f) { EndGame__(ESurvivalEndReason::ShipDestroyed); return true; }
	this->BeginDay__();
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
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->PrepareDay (CurrentMap, DayPreparationTimeout, MinimumInitialSpawnRatio);
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
	if (CurrentDay > 1) { ApplyDailySettlement__(); }
	if (State == ESurvivalState::GameOver) return;
	RemainingDayTime = DayDuration;
	UpdateGravity__();
	if (CurrentDay % 3 == 0)
	{
		const float Damage = FMath::Max(0.0f, FirstSolarWindDamage) + (CurrentDay / 3 - 1) * FMath::Max(0.0f, SolarWindDamageIncrease);
		IDurabilityInterface::Execute_ConsumDurability(SpaceShip, Damage);
	}
	if (State != ESurvivalState::GameOver)
	{
		if (bTransitionFromCompletedDay__) StartDayFadeIn__();
		else EnterPlayableDay__();
	}
}

void ASurvivalLoopActor::EnterPlayableDay__()
{
	if (State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) return;
	State = ESurvivalState::Playing;
	SetPreparationInputLocked__(false);
	OnDayStarted.Broadcast(CurrentDay, CurrentMap);
	if (FinalDay > 0 && CurrentDay >= FinalDay) ClearGame__();
}

void ASurvivalLoopActor::FailDayPreparation__(const FString& Reason)
{
	DayPreparationError = Reason;
	State = ESurvivalState::PreparationFailed;
	if (USpaceSalvageWorldSubsystem* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>())
	{
		Salvage->CancelDayPreparation();
	}
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	bTransitionFromCompletedDay__ = false;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, DayFadeInDuration, FLinearColor::Black, false, false);
	}
	SetPreparationInputLocked__(false);
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
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) { return; }
	if (!IsValid(Player)) { NotifyPlayerDeath(); return; }
	if (!IsValid(SpaceShip)) { EndGame__(ESurvivalEndReason::ShipDestroyed); return; }
	if (State == ESurvivalState::PreparationFailed) return;
	if (State == ESurvivalState::WaitingForMeteor)
	{
		DayFadeOutRemaining__ = FMath::Max(0.0f, DayFadeOutRemaining__ - FMath::Max(0.0f, DeltaSeconds));
		if (DayFadeOutRemaining__ <= 0.0f) HandleDayFadeOutComplete__();
		return;
	}
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
}

void ASurvivalLoopActor::FinishDay()
{
	if (State != ESurvivalState::Playing) { return; }
	RemainingDayTime = 0.0f;
	State = ESurvivalState::WaitingForMeteor;
	SetPreparationInputLocked__(true);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->EndOfDay();

	const float FadeDuration = FMath::Max(0.0f, DayFadeOutDuration);
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeDuration, FLinearColor::Black, false, true);
	}
	// Tick owns this countdown. Even a zero-duration transition completes on the
	// next tick, preserving an observable and re-entry-safe WaitingForMeteor state.
	DayFadeOutRemaining__ = FadeDuration;
}

void ASurvivalLoopActor::HandleDayFadeOutComplete__()
{
	if (State != ESurvivalState::WaitingForMeteor) return;

	USpaceSalvageWorldSubsystem* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (!Salvage)
	{
		FailDayPreparation__(TEXT("SpaceSalvageWorldSubsystem is missing during day transition."));
		return;
	}
	Salvage->ClearDayActors();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>())
		{
			SaveSubsystem->SaveCompletedDay(this);
		}
	}

	RecenterPlayer__();
	bTransitionFromCompletedDay__ = true;
	BeginDay__();
}

void ASurvivalLoopActor::RecenterPlayer__()
{
	if (!IsValid(Player) || !IsValid(InteriorBounds)) return;
	if (UCharacterMovementComponent* Movement = Player->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
	Player->SetActorLocation(InteriorBounds->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
}

void ASurvivalLoopActor::StartDayFadeIn__()
{
	const float FadeDuration = FMath::Max(0.0f, DayFadeInDuration);
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, FadeDuration, FLinearColor::Black, false, false);
	}
	if (FadeDuration <= 0.0f)
	{
		HandleDayFadeInComplete__();
	}
	else
	{
		GetWorldTimerManager().SetTimer(DayFadeInTimer__, this,
			&ThisClass::HandleDayFadeInComplete__, FadeDuration, false);
	}
}

void ASurvivalLoopActor::HandleDayFadeInComplete__()
{
	bTransitionFromCompletedDay__ = false;
	EnterPlayableDay__();
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
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) { return; }
	State = ESurvivalState::GameOver;
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	DayFadeOutRemaining__ = 0.0f;
	bTransitionFromCompletedDay__ = false;
	SetPreparationInputLocked__(false);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameOver.Broadcast(Reason, CurrentDay);
	UGameplayStatics::SetGamePaused(this, true);
	ShowGameOverWidget__();
}

void ASurvivalLoopActor::ShowGameOverWidget__()
{
	if (IsValid(GameOverWidgetInstance__)) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController) || !GameOverWidgetClass)
	{
		// Headless automation worlds intentionally have no local player controller.
		UE_LOG(LogTemp, Warning, TEXT("Cannot show game over UI: PlayerController or GameOverWidgetClass is invalid."));
		return;
	}

	GameOverWidgetInstance__ = CreateWidget<UUserWidget>(PlayerController, GameOverWidgetClass);
	if (!IsValid(GameOverWidgetInstance__))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create WBP_GameOver."));
		return;
	}

	UButton* MainMenuButton = Cast<UButton>(GameOverWidgetInstance__->GetWidgetFromName(TEXT("GoToMainButton")));
	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleGoToMainMenu__);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WBP_GameOver requires a Button named 'GoToMainButton'."));
	}

	GameOverWidgetInstance__->AddToViewport(1000);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(GameOverWidgetInstance__->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
}

void ASurvivalLoopActor::HandleGoToMainMenu__()
{
	if (MainMenuLevel.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuLevel is not configured."));
		return;
	}

	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, MainMenuLevel);
}

void ASurvivalLoopActor::ClearGame__()
{
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) { return; }
	State = ESurvivalState::Cleared;
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	DayFadeOutRemaining__ = 0.0f;
	bTransitionFromCompletedDay__ = false;
	SetPreparationInputLocked__(false);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameCleared.Broadcast(CurrentDay);
	UGameplayStatics::SetGamePaused(this, true);
}

void ASurvivalLoopActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	SetPreparationInputLocked__(false);
	if (IsValid(SpaceShip)) { SpaceShip->OnDurabilityChange.RemoveDynamic(this, &ASurvivalLoopActor::HandleDurability__); }
	if (IsValid(Player))
	{
		if (auto* Stats = Player->FindComponentByClass<UStatComponent>()) { Stats->OnPlayerDeath.RemoveDynamic(this, &ASurvivalLoopActor::NotifyPlayerDeath); }
	}
	if (auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>())
	{
		if (Salvage->GetSurvivalLoop() == this)
		{
			Salvage->StopSurvival();
			Salvage->UnregisterSurvivalLoopActor(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}
