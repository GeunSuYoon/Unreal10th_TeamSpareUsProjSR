#include "Framework/SurvivalLoopActor.h"
#include "Camera/ImpactCameraShake.h"
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
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Widget/SRMainHUD.h"

ASurvivalLoopActor::ASurvivalLoopActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SolarWindCameraShake = USolarWindCameraShake::StaticClass();
	InteriorBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteriorBounds"));
	SetRootComponent(InteriorBounds);
	// Includes the playable cabin through the airlock controls at roughly Y=-980.
	InteriorBounds->SetBoxExtent(FVector(600.0f, 1100.0f, 220.0f));
	InteriorBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverClassFinder(
		TEXT("/Game/Blueprint/GeunSuYoon/Widget/MainPanel/WBP_GameOver"));
	if (GameOverClassFinder.Succeeded())
	{
		GameOverWidgetClass = GameOverClassFinder.Class;
		GameClearWidgetClass = GameOverClassFinder.Class;
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
		|| !FMath::IsFinite(LowEnergyOxygenRatio) || !FMath::IsFinite(DailyHealthRecoveryRatio)
		|| (Salvage->GetSurvivalLoop() && Salvage->GetSurvivalLoop() != this))
	{
		UE_LOG(LogTemp, Error, TEXT("Survival setup invalid: player stats must be initialized; assign ship, player, day-one map and positive finite oxygen capacity/drain; use one loop actor."));
		return false;
	}
	SpaceShip->OnDurabilityChange.AddUniqueDynamic(this, &ASurvivalLoopActor::HandleDurability__);
	Player->FindComponentByClass<UStatComponent>()->OnPlayerDeath.AddUniqueDynamic(this, &ASurvivalLoopActor::NotifyPlayerDeath);
	AttachToActor(SpaceShip, FAttachmentTransformRules::KeepWorldTransform);
	UGameplayStatics::SetGamePaused(this, false);
	// Headless automation worlds have no local controller. A real play session
	// restores game input here after travelling from the UI-only main menu.
	if (APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()))
	{
		PlayerController->ResetIgnoreInputFlags();
		PlayerController->FlushPressedKeys();
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		Player->EnableInput(PlayerController);
		//CastChecked<ASRMainHUD>(PlayerController->GetHUD())->RegisterSurvivalLoop(this);
	}
	State = ESurvivalState::Playing;
	if (Player->FindComponentByClass<UStatComponent>()->GetHealth() <= 0.0f) { NotifyPlayerDeath(); return true; }
	if (SpaceShip->GetCurrentDurability() <= 0.0f) { EndGame__(ESurvivalEndReason::ShipDestroyed); return true; }
	this->BeginDay__();
	return true;
}

void ASurvivalLoopActor::BeginDay__()
{
	int32 BestDay = 0;
	CurrentMap = nullptr;
	for (const auto& Entry : Maps)
	{
		if (Entry.StartDay <= CurrentDay + 1 && Entry.StartDay > BestDay) { BestDay = Entry.StartDay; CurrentMap = Entry.MapData; }
	}

	++CurrentDay;
	if (CurrentDay > 1) { ApplyDailySettlement__(); }
	if (State == ESurvivalState::GameOver) return;

	this->InitDelegate();
	UpdateGravity__();
	if (CurrentDay % 3 == 0)
	{
		const float Damage = FMath::Max(0.0f, FirstSolarWindDamage) + (CurrentDay / 3 - 1) * FMath::Max(0.0f, SolarWindDamageIncrease);
		if (SolarWindSFX)
		{
			UGameplayStatics::PlaySound2D(this, SolarWindSFX);
		}
		TSubclassOf<UCameraShakeBase> CameraShakeClass = SolarWindCameraShake;
		if (!CameraShakeClass)
		{
			CameraShakeClass = USolarWindCameraShake::StaticClass();
		}
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		checkf(PC && PC->PlayerCameraManager, TEXT("Solar wind camera shake requires a player camera manager."));
		PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
		PendingShipDestructionReason__ = ESurvivalEndReason::SolarWind;
		IDurabilityInterface::Execute_ConsumDurability(SpaceShip, Damage);
		PendingShipDestructionReason__ = ESurvivalEndReason::ShipDestroyed;
	}
	if (State == ESurvivalState::GameOver) return;

	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StartDay(CurrentMap);
	if (bTransitionFromCompletedDay__) StartDayFadeIn__();
	else EnterPlayableDay__();
}
void ASurvivalLoopActor::EnterPlayableDay__()
{
	if (State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) return;
	State = ESurvivalState::Playing;
	SetTransitionInputLocked__(false);
	if (FinalDay > 0 && CurrentDay >= FinalDay) ClearGame__();
}

void ASurvivalLoopActor::SetTransitionInputLocked__(bool bLocked)
{
	if (bTransitionInputLocked__ == bLocked) return;
	bTransitionInputLocked__ = bLocked;
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
	if (State == ESurvivalState::WaitingForMeteor)
	{
		DayFadeOutRemaining__ = FMath::Max(0.0f, DayFadeOutRemaining__ - FMath::Max(0.0f, DeltaSeconds));
		if (DayFadeOutRemaining__ <= 0.0f) HandleDayFadeOutComplete__();
		return;
	}
	UpdateGravity__();
}

int32 ASurvivalLoopActor::GetNextSolarWindDay__() const
{
	return (CurrentDay / 3 + 1) * 3;
}

float ASurvivalLoopActor::GetNextSolarWindDamage__() const
{
	const int32 NextSolarWindIndex = GetNextSolarWindDay__() / 3 - 1;
	return FMath::Max(0.0f, FirstSolarWindDamage)
		+ NextSolarWindIndex * FMath::Max(0.0f, SolarWindDamageIncrease);
}

void ASurvivalLoopActor::FinishDay()
{
	if (State != ESurvivalState::Playing) { return; }
	State = ESurvivalState::WaitingForMeteor;
	SetTransitionInputLocked__(true);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->EndOfDay();

	const float FadeDuration = FMath::Max(0.0f, DayFadeOutDuration);
	OnDayFadeOut.Broadcast();
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
	checkf(Salvage, TEXT("SpaceSalvageWorldSubsystem is required during day transition."));

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>())
		{
			if (!SaveSubsystem->SaveCompletedDay(this))
			{
				UE_LOG(LogTemp, Error, TEXT("Completed the day, but the checkpoint could not be saved. Select next day to retry."));
				State = ESurvivalState::Playing;
				Salvage->StartDay(CurrentMap);
				OnDayFadeIn.Broadcast();
				if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); PC && PC->PlayerCameraManager)
				{
					PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, DayFadeInDuration, FLinearColor::Black, false, false);
				}
				SetTransitionInputLocked__(false);
				return;
			}
		}
	}

	Salvage->ClearDayActors();
	RecenterPlayer__();
	bTransitionFromCompletedDay__ = true;
	State = ESurvivalState::Playing;
	this->InitDelegate();
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
	OnDayFadeIn.Broadcast();

	
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

void ASurvivalLoopActor::NotifyMeteorImpact(ASpaceShipActor* HitShip, float Damage)
{
	if (HitShip != SpaceShip || (State != ESurvivalState::Playing && State != ESurvivalState::WaitingForMeteor)) { return; }

	PendingShipDestructionReason__ = ESurvivalEndReason::MeteorCollision;
	if (!IsPlayerSafe() && IsValid(Player))
	{
		PendingPlayerDeathReason__ = ESurvivalEndReason::MeteorCollision;
		if (auto* Stats = Player->FindComponentByClass<UStatComponent>())
		{
			Stats->ModifyHealth(-Stats->GetHealth());
		}
		NotifyPlayerDeath();
	}
	UGameplayStatics::ApplyDamage(HitShip, Damage, nullptr, nullptr, nullptr);
	PendingShipDestructionReason__ = ESurvivalEndReason::ShipDestroyed;
}

void ASurvivalLoopActor::InitDelegate()
{
	//OnDayStarted.Broadcast(CurrentDay, CurrentMap);
	const int32 NextSolarWindDay = GetNextSolarWindDay__();
	const float NextSolarWindDamage = GetNextSolarWindDamage__();

	OnDayStatusChanged.Broadcast(CurrentDay, NextSolarWindDay - CurrentDay, NextSolarWindDamage);
	OnRequiredDurabilityChanged.Broadcast(NextSolarWindDamage);
	OnDayStarted.Broadcast(CurrentDay, CurrentMap);
}

void ASurvivalLoopActor::NotifyPlayerDeath()
{
	const ESurvivalEndReason Reason = PendingPlayerDeathReason__;
	PendingPlayerDeathReason__ = ESurvivalEndReason::PlayerDied;
	EndGame__(Reason);
}

void ASurvivalLoopActor::HandleDurability__(float Current, float Maximum)
{
	if (Current <= 0.0f)
	{
		const ESurvivalEndReason Reason = PendingShipDestructionReason__;
		PendingShipDestructionReason__ = ESurvivalEndReason::ShipDestroyed;
		EndGame__(Reason);
	}
}

void ASurvivalLoopActor::EndGame__(ESurvivalEndReason Reason)
{
	if (State == ESurvivalState::Ready || State == ESurvivalState::GameOver || State == ESurvivalState::Cleared) { return; }
	State = ESurvivalState::GameOver;
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	DayFadeOutRemaining__ = 0.0f;
	bTransitionFromCompletedDay__ = false;
	SetTransitionInputLocked__(false);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameOver.Broadcast(Reason, CurrentDay);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>())
		{
			SaveSubsystem->DeleteSaveGame();
		}
	}
	UGameplayStatics::SetGamePaused(this, true);
	ShowResultWidget__(false, Reason);
}

void ASurvivalLoopActor::ShowResultWidget__(bool bGameCleared, ESurvivalEndReason Reason)
{
	if (IsValid(GameOverWidgetInstance__)) return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	TSubclassOf<UUserWidget> ResultWidgetClass = bGameCleared ? GameClearWidgetClass : GameOverWidgetClass;
	if (!ResultWidgetClass)
	{
		ResultWidgetClass = GameOverWidgetClass;
	}
	if (!IsValid(PlayerController) || !ResultWidgetClass)
	{
		// Headless automation worlds intentionally have no local player controller.
		UE_LOG(LogTemp, Warning, TEXT("Cannot show result UI: PlayerController or result widget class is invalid."));
		return;
	}

	GameOverWidgetInstance__ = CreateWidget<UUserWidget>(PlayerController, ResultWidgetClass);
	if (!IsValid(GameOverWidgetInstance__))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create the game result widget."));
		return;
	}

	FText ResultMessage;
	FText DescriptionMessage;
	if (bGameCleared)
	{
		ResultMessage = FText::FromString(TEXT("게임 클리어"));
		DescriptionMessage = FText::Format(
			FText::FromString(TEXT("{0}일 동안 생존했습니다. 모든 생존 목표를 달성했습니다.")),
			FText::AsNumber(CurrentDay));
	}
	else
	{
		switch (Reason)
		{
		case ESurvivalEndReason::MeteorCollision:
			ResultMessage = FText::FromString(TEXT("운석 충돌로 사망했습니다."));
			DescriptionMessage = FText::FromString(
				TEXT("운석이 감지되면 메인 패널에서 운석을 회피하거나 운석 충돌 전 우주선 문을 닫아야 합니다."));
			break;
		case ESurvivalEndReason::SolarWind:
			ResultMessage = FText::FromString(TEXT("태양풍으로 우주선이 파괴되었습니다."));
			DescriptionMessage = FText::FromString(
				TEXT("태양풍이 발생하기 전에 우주선을 수리하여 예상 태양풍 피해보다 높은 내구도를 확보해야 합니다."));
			break;
		case ESurvivalEndReason::ShipDestroyed:
			ResultMessage = FText::FromString(TEXT("우주선이 파괴되었습니다."));
			DescriptionMessage = FText::FromString(
				TEXT("우주선 내구도를 자주 확인하고 위험한 이벤트가 발생하기 전에 우주선을 수리해야 합니다."));
			break;
		case ESurvivalEndReason::PlayerDied:
		default:
			ResultMessage = FText::FromString(TEXT("생존에 실패했습니다."));
			DescriptionMessage = FText::FromString(
				TEXT("체력과 산소를 확인하고, 우주선 밖으로 나가기 전에 우주복과 산소를 충분히 준비해야 합니다."));
			break;
		}
	}

	CastChecked<UTextBlock>(GameOverWidgetInstance__->GetWidgetFromName(TEXT("ResultText")))->SetText(ResultMessage);
	CastChecked<UTextBlock>(GameOverWidgetInstance__->GetWidgetFromName(TEXT("Description")))->SetText(DescriptionMessage);
	UButton* MainMenuButton = CastChecked<UButton>(GameOverWidgetInstance__->GetWidgetFromName(TEXT("GoToMainButton")));
	MainMenuButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleGoToMainMenu__);

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
	SetTransitionInputLocked__(false);
	GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>()->StopSurvival();
	if (IsValid(SpaceShip) && SpaceShip->GetMeteorAvoidance()) { SpaceShip->GetMeteorAvoidance()->ClearMeteor(); }
	OnGameCleared.Broadcast(CurrentDay);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>())
		{
			SaveSubsystem->DeleteSaveGame();
		}
	}
	UGameplayStatics::SetGamePaused(this, true);
	ShowResultWidget__(true);
}

void ASurvivalLoopActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(DayFadeInTimer__);
	SetTransitionInputLocked__(false);
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
