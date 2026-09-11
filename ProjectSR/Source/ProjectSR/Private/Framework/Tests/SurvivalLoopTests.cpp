#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Framework/SurvivalLoopActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "SpaceShip/SpaceShipActor.h"
#include "Player/PlayerCharacter.h"
#include "Component/StatComponent.h"
#include "Component/InSpaceMovementComponent.h"
#include "Data/SpaceMap/SpaceMapDataAsset.h"
#include "Item/MeteorItemActor.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Data/ObjectPool/ObjectPoolDataAsset.h"
#include "UObject/Script.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "Framework/Subsystem/ItemActorFactorySubsystem.h"
#include "GameFramework/PlayerController.h"

// Wait for the real streamable-manager callback instead of assuming request == completion.
class FWaitForPreparedDay final : public IAutomationLatentCommand
{
public:
	FWaitForPreparedDay(UWorld* InWorld, ASurvivalLoopActor* InLoop, FAutomationTestBase* InTest)
		: World(InWorld), Loop(InLoop), Test(InTest), Deadline(FPlatformTime::Seconds() + 10.0) {}
	virtual bool Update() override
	{
		Loop->Tick(0.0f);
		if (Loop->State == ESurvivalState::PreparingDay && FPlatformTime::Seconds() < Deadline) return false;
		Test->TestTrue(TEXT("Real async initial spawning completes"), Loop->State == ESurvivalState::Playing);
		int32 Count = 0;
		for (TActorIterator<AItemActor> It(World); It; ++It)
		{
			if (!It->IsHidden()) ++Count;
		}
		Test->TestEqual(TEXT("Late cancelled callbacks do not add/recycle actors"), Count, 8);
		Test->TestEqual(TEXT("Async retry advances one day only"), Loop->CurrentDay, 3);
		Test->TestEqual(TEXT("Async retry charges once"), Loop->SpaceShip->GetCurrentEnergy(), 10.0f);
		World->DestroyWorld(false);
		return true;
	}
private:
	UWorld* World;
	ASurvivalLoopActor* Loop;
	FAutomationTestBase* Test;
	double Deadline;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSurvivalLoopTest, "ProjectSR.Survival.ProgressionAndDeath", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FSurvivalLoopTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("World"), World)) { return false; }
	auto* Ship = World->SpawnActor<ASpaceShipActor>();
	auto* Player = World->SpawnActor<APlayerCharacter>();
	auto* Loop = World->SpawnActor<ASurvivalLoopActor>();
	Player->DispatchBeginPlay();
	auto* Salvage = World->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (!Ship || !Player || !Loop || !Salvage) { AddError(TEXT("Test setup failed")); World->DestroyWorld(false); return false; }
	FSpaceShipStat Stat;
	Stat.MaxDurability = 1000.0f;
	Ship->ApplySpaceShipStat(Stat);
	Ship->RepairDurability_Implementation(1000.0f);
	Player->FindComponentByClass<UStatComponent>()->ModifyHealth(100.0f);
	Salvage->RegisterSpaceShipActor(Ship);
	auto* FirstMap = NewObject<USpaceMapDataAsset>(Loop);
	auto* SecondMap = NewObject<USpaceMapDataAsset>(Loop);
	FSurvivalMapEntry First; First.StartDay = 1; First.MapData = FirstMap;
	FSurvivalMapEntry Second; Second.StartDay = 3; Second.MapData = SecondMap;
	Loop->Maps = { Second, First };
	Loop->SpaceShip = Ship;
	Loop->Player = Player;
	Player->SetActorLocation(Loop->GetActorLocation());
	TestTrue(TEXT("Start succeeds with unordered schedule"), Loop->StartSurvival());
	TestFalse(TEXT("Cannot start twice"), Loop->StartSurvival());
	Loop->Tick(0.0f);
	TestEqual(TEXT("Starts on day one"), Loop->CurrentDay, 1);
	TestTrue(TEXT("Day one map selected"), Loop->CurrentMap == FirstMap);
	TestTrue(TEXT("Closed cabin has gravity"), Player->GetInSpaceMovementComponent()->GetGravityState() == EGravityState::GravityMode);
	for (int32 Day = 2; Day <= 10; ++Day)
	{
		Loop->Tick(Loop->DayDuration);
		TestTrue(TEXT("Deadline enters transition"), Loop->State == ESurvivalState::WaitingForMeteor);
		Loop->Tick(0.01f);
	Loop->Tick(0.0f);
		TestEqual(TEXT("Day advances once"), Loop->CurrentDay, Day);
		if (Day >= 3) { TestTrue(TEXT("Latest eligible map retained"), Loop->CurrentMap == SecondMap); }
		if (Day == 2) { TestEqual(TEXT("No early wind"), Ship->GetCurrentDurability(), 1000.0f); }
		if (Day == 3) { TestEqual(TEXT("First wind"), Ship->GetCurrentDurability(), 990.0f); }
		if (Day == 6) { TestEqual(TEXT("Second wind increases to fifteen"), Ship->GetCurrentDurability(), 975.0f); }
	}
	TestEqual(TEXT("Third wind increases to twenty; no extra wind on day ten"), Ship->GetCurrentDurability(), 955.0f);
	Loop->NotifyMeteorImpact(Ship);
	TestTrue(TEXT("Inside player survives impact notification"), Loop->State == ESurvivalState::Playing);
	Player->SetActorLocation(FVector(10000.0f));
	Loop->Tick(0.01f);
	Loop->Tick(0.0f);
	TestTrue(TEXT("Outside has zero gravity"), Player->GetInSpaceMovementComponent()->GetGravityState() == EGravityState::ZeroGravityMode);
	Loop->NotifyMeteorImpact(Ship);
	TestTrue(TEXT("Outside impact ends game"), Loop->State == ESurvivalState::GameOver);
	TestEqual(TEXT("Outside impact kills player"), Player->FindComponentByClass<UStatComponent>()->GetHealth(), 0.0f);
	Loop->FinishDay();
	Loop->Tick(1000.0f);
	TestEqual(TEXT("Game over freezes date"), Loop->CurrentDay, 10);
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSurvivalDailyTest, "ProjectSR.Survival.DailyResources", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FSurvivalDailyTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto* Ship = World->SpawnActor<ASpaceShipActor>();
	auto* Player = World->SpawnActor<APlayerCharacter>();
	auto* Loop = World->SpawnActor<ASurvivalLoopActor>();
	Player->DispatchBeginPlay();
	auto* Stats = Player->FindComponentByClass<UStatComponent>();
	FSpaceShipStat ShipStat;
	ShipStat.MaxDurability = 1000.0f;
	ShipStat.MaxEnergy = 10.0f;
	ShipStat.OperationalEnergy = 10.0f;
	Ship->ApplySpaceShipStat(ShipStat);
	World->GetSubsystem<USpaceSalvageWorldSubsystem>()->RegisterSpaceShipActor(Ship);
	FSurvivalMapEntry Entry;
	Entry.StartDay = 1;
	Entry.MapData = NewObject<USpaceMapDataAsset>(Loop);
	Loop->Maps = { Entry };
	Loop->SpaceShip = Ship;
	Loop->Player = Player;
	Player->SetActorLocation(Loop->GetActorLocation());
	TestTrue(TEXT("Start daily test"), Loop->StartSurvival());
	Loop->Tick(0.0f);
	TestEqual(TEXT("Day duration is independently configured"), Loop->DayDuration, 300.0f);
	TestEqual(TEXT("First day does not charge energy"), Ship->GetCurrentEnergy(), 10.0f);
	Stats->ModifyOxygen(-149.5f);
	Stats->SetOxygenConsuming(true);
	Stats->TickComponent(1.0f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("Closed cabin neither drains nor recovers oxygen"), Stats->GetOxygen(), 0.5f);
	Player->SetActorLocation(FVector(10000.0f));
	Stats->SetOxygenConsuming(false);
	Stats->TickComponent(1.0f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("Outside overrides gravity callback and consumes oxygen"), Stats->GetOxygen(), 0.0f);
	TestEqual(TEXT("Only half a second causes suffocation"), Stats->GetHealth(), 95.0f);
	Player->SetActorLocation(Loop->GetActorLocation());
	Stats->TickComponent(1.0f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("Closed cabin stops suffocation at zero oxygen"), Stats->GetHealth(), 95.0f);
	Stats->ModifyHealth(-75.0f);
	Loop->FinishDay();
	Loop->Tick(0.01f);
	Loop->Tick(0.0f);
	TestTrue(TEXT("Exact operating cost fully powers recovery"), Loop->bLastDailyEnergySufficient);
	TestEqual(TEXT("Operating cost charged"), Ship->GetCurrentEnergy(), 0.0f);
	TestEqual(TEXT("Full oxygen refill"), Stats->GetOxygen(), 150.0f);
	TestEqual(TEXT("Recover half maximum HP, not set HP to half"), Stats->GetHealth(), 70.0f);
	Stats->ModifyOxygen(-150.0f);
	Ship->GainEnergy(9.0f);
	FEquipmentStatModifier Suit;
	Suit.OxygenBonus = 30.0f;
	Stats->RecalculateMaxStats(Suit);
	TestEqual(TEXT("Suit does not grant free oxygen"), Stats->GetOxygen(), 0.0f);
	TestEqual(TEXT("Suit does not extend current day"), Loop->DayDuration, 300.0f);
	Loop->FinishDay();
	Loop->Tick(0.01f);
	Loop->Tick(0.0f);
	TestFalse(TEXT("Insufficient energy limits recovery"), Loop->bLastDailyEnergySufficient);
	TestEqual(TEXT("Partial available energy is spent"), Ship->GetCurrentEnergy(), 0.0f);
	TestEqual(TEXT("Suit still does not extend the next day"), Loop->DayDuration, 300.0f);
	TestEqual(TEXT("Low energy fills 70 percent of upgraded capacity"), Stats->GetOxygen(), 126.0f);
	TestEqual(TEXT("HP recovery clamps to maximum"), Stats->GetHealth(), 100.0f);
	Stats->ModifyOxygen(44.0f);
	Loop->FinishDay();
	Loop->FinishDay();
	Loop->Tick(0.01f);
	Loop->Tick(0.0f);
	TestEqual(TEXT("Repeated finish does not skip days"), Loop->CurrentDay, 4);
	TestEqual(TEXT("Low energy never removes remaining oxygen"), Stats->GetOxygen(), 170.0f);
	// Exercise the same reflected door action used by the button, including closing animation.
	UFunction* DoorAction = Ship->FindFunction(TEXT("DetectDoorButtonClick_"));
	if (TestNotNull(TEXT("Door action"), DoorAction))
	{
		Ship->ProcessEvent(DoorAction, nullptr);
		TestFalse(TEXT("Inside with open door is unsafe"), Loop->IsPlayerSafe());
		Stats->TickComponent(1.0f, LEVELTICK_All, nullptr);
		TestEqual(TEXT("Open cabin drains oxygen"), Stats->GetOxygen(), 169.0f);
		Ship->ProcessEvent(DoorAction, nullptr);
		TestFalse(TEXT("Closing door is still unsafe"), Loop->IsPlayerSafe());
		Loop->NotifyMeteorImpact(Ship);
		TestTrue(TEXT("Impact in unsealed cabin ends game"), Loop->State == ESurvivalState::GameOver);
		Stats->ApplyDailyRecovery(1.0f, 0.5f);
		TestEqual(TEXT("Daily recovery cannot revive player"), Stats->GetHealth(), 0.0f);
	}
	World->DestroyWorld(false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSurvivalPreparationTest, "ProjectSR.Survival.DayPreparation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FSurvivalPreparationTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto* Ship = World->SpawnActor<ASpaceShipActor>();
	auto* Player = World->SpawnActor<APlayerCharacter>();
	auto* PC = World->SpawnActor<APlayerController>();
	PC->Possess(Player);
	Player->DispatchBeginPlay();
	auto* Loop = World->SpawnActor<ASurvivalLoopActor>();
	auto* Salvage = World->GetSubsystem<USpaceSalvageWorldSubsystem>();
	auto* Stats = Player->FindComponentByClass<UStatComponent>();
	FSpaceShipStat ShipStat;
	ShipStat.MaxDurability = 1000.0f;
	ShipStat.MaxEnergy = 30.0f;
	ShipStat.OperationalEnergy = 10.0f;
	Ship->ApplySpaceShipStat(ShipStat);
	Salvage->RegisterSpaceShipActor(Ship);
	Ship->SetActorLocation(FVector(20000.0f, 5000.0f, 0.0f));
	Player->SetActorLocation(FVector(30000.0f));
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!TestNotNull(TEXT("Test mesh"), Mesh)) { World->DestroyWorld(false); return false; }
	auto* Item = NewObject<UItemDataAsset>(Loop);
	Item->ItemActorClass = AItemActor::StaticClass();
	Item->Mesh = Mesh;
	auto* PoolData = NewObject<UObjectPoolDataAsset>(Loop);
	PoolData->ActorClass = AItemActor::StaticClass();
	PoolData->MaxSize = 8;
	PoolData->MaxPolicy = EObjectPoolPolicy::DoNotSpawn;
	World->GetSubsystem<UObjectPoolSubsystem>()->RegisterPoolDataAsset(PoolData);
	auto* Map = NewObject<USpaceMapDataAsset>(Loop);
	Map->ItemSpawnInitCount = 10;
	Map->ItemSpawnDist = 5000.0f;
	Map->ItemMoveSpeed = 100.0f;
	Map->ItemSpawnTime = 1.0f;
	Map->ItemSpawnRate.Add(Item, 1);
	FSurvivalMapEntry Entry;
	Entry.StartDay = 1;
	Entry.MapData = Map;
	Loop->Maps = { Entry };
	Loop->Player = Player;
	Loop->SpaceShip = Ship;
	TestTrue(TEXT("Preparation accepted"), Loop->StartSurvival());
	TestTrue(TEXT("Start enters preparation, not play"), Loop->State == ESurvivalState::PreparingDay);
	TestEqual(TEXT("No day committed before activation"), Loop->CurrentDay, 0);
	TestEqual(TEXT("No countdown during preparation"), Loop->RemainingDayTime, 0.0f);
	TestFalse(TEXT("Pawn input blocked"), Player->InputEnabled());
	TestTrue(TEXT("Controller look blocked"), PC->IsLookInputIgnored());
	TestFalse(TEXT("Movement tick frozen"), Player->GetInSpaceMovementComponent()->IsComponentTickEnabled());
	Stats->ModifyOxygen(-150.0f);
	Stats->TickComponent(10.0f, LEVELTICK_All, nullptr);
	TestEqual(TEXT("Preparation at zero oxygen causes no HP damage"), Stats->GetHealth(), 100.0f);
	int32 Count = 0;
	for (TActorIterator<AItemActor> It(World); It; ++It)
	{
		if (It->IsHidden()) continue;
		++Count;
		TestFalse(TEXT("Initial actor does not tick"), It->IsActorTickEnabled());
		TestFalse(TEXT("Initial actor cannot be collected"), It->GetActorEnableCollision());
		const float Distance = FVector::Distance(It->GetActorLocation(), Ship->GetActorLocation());
		TestTrue(TEXT("Placement is ship-relative and outside safe area"), Distance > 500.0f && Distance <= 5001.0f);
	}
	TestEqual(TEXT("Eighty percent of requested initial actors exist before play"), Count, 8);
	TestEqual(TEXT("Synchronous callbacks also count correctly"), Loop->GetDayPreparationProgress(), 1.0f);
	Loop->Tick(5.0f);
	TestTrue(TEXT("Prepared day starts"), Loop->State == ESurvivalState::Playing);
	TestEqual(TEXT("Preparation delta does not shorten day"), Loop->RemainingDayTime, 150.0f);
	TestTrue(TEXT("Pawn input restored"), Player->InputEnabled());
	TestFalse(TEXT("Controller look restored"), PC->IsLookInputIgnored());
	for (TActorIterator<AItemActor> It(World); It; ++It)
	{
		if (It->IsHidden()) continue;
		TestTrue(TEXT("Actor activated"), It->IsActorTickEnabled() && It->GetActorEnableCollision());
	}
	// A missing class fails without committing the day or paying its operating cost.
	Item->ItemActorClass.Reset();
	Loop->FinishDay();
	Loop->Tick(0.0f);
	Loop->Tick(0.0f);
	TestTrue(TEXT("Invalid initial asset reports failure"), Loop->State == ESurvivalState::PreparationFailed);
	TestEqual(TEXT("Failed preparation does not advance day"), Loop->CurrentDay, 1);
	TestEqual(TEXT("Failed preparation does not charge energy"), Ship->GetCurrentEnergy(), 30.0f);
	TestTrue(TEXT("Failure has readable reason"), !Loop->DayPreparationError.IsEmpty());
	Item->ItemActorClass = AItemActor::StaticClass();
	TestTrue(TEXT("Retry accepted"), Loop->RetryDayPreparation());
	Loop->Tick(0.0f);
	TestEqual(TEXT("Retry commits same next day"), Loop->CurrentDay, 2);
	TestEqual(TEXT("Retry charges exactly once"), Ship->GetCurrentEnergy(), 20.0f);
	// A denied factory request does not allocate an actor, even if its assets are loaded.
	Salvage->CancelDayPreparation();
	bool bCalled = false;
	World->GetSubsystem<UItemActorFactorySubsystem>()->SpawnItemActorAsync(Item, FTransform::Identity,
		FOnPickupSpawned::CreateLambda([&bCalled, this](AItemActor* Spawned)
		{
			bCalled = true;
			TestNull(TEXT("Cancelled request never allocates a pooled actor"), Spawned);
		}), []() { return false; });
	TestTrue(TEXT("Cancelled callback resolved"), bCalled);
	// Select an on-disk mesh not yet loaded to exercise real asynchronous requests.
	for (const TCHAR* Path : { TEXT("/Engine/BasicShapes/Cube1.Cube1"), TEXT("/Engine/BasicShapes/Cone.Cone"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder") })
	{
		TSoftObjectPtr<UStaticMesh> Candidate{FSoftObjectPath(Path)};
		if (!Candidate.IsValid()) { Item->Mesh = Candidate; break; }
	}
	TestFalse(TEXT("Async test mesh starts unloaded"), Item->Mesh.IsValid());
	Loop->FinishDay();
	Loop->Tick(0.0f);
	TestTrue(TEXT("Unloaded mesh remains preparing"), Salvage->GetDayPreparationStatus() == EDayPreparationStatus::Preparing);
	Salvage->Tick(Loop->DayPreparationTimeout + 1.0f);
	Loop->Tick(0.0f);
	TestTrue(TEXT("Timeout becomes retryable failure"), Loop->State == ESurvivalState::PreparationFailed);
	TestEqual(TEXT("Timeout does not charge operating energy"), Ship->GetCurrentEnergy(), 20.0f);
	TestTrue(TEXT("Retry starts fresh async generation"), Loop->RetryDayPreparation());
	ADD_LATENT_AUTOMATION_COMMAND(FWaitForPreparedDay(World, Loop, this));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSurvivalMeteorPoolTest, "ProjectSR.Survival.MeteorLifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FSurvivalMeteorPoolTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	FEditorScriptExecutionGuard ScriptGuard;
#endif
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	auto* Pool = World->GetSubsystem<UObjectPoolSubsystem>();
	auto* PoolData = NewObject<UObjectPoolDataAsset>();
	PoolData->ActorClass = AMeteorItemActor::StaticClass();
	PoolData->MaxSize = 2;
	Pool->RegisterPoolDataAsset(PoolData);
	auto* Meteor = Cast<AMeteorItemActor>(Pool->SpawnFromPool(TSubclassOf<AActor>(AMeteorItemActor::StaticClass()), FTransform::Identity));
	if (!Meteor) { World->DestroyWorld(false); return false; }
	FMeteor Data;
	Data.MoveDir = FVector::ForwardVector;
	Data.MeteorDamage = 10.0f;
	Data.MeteorSize = 10.0f;
	Meteor->SetActorLocation(FVector(-1000.0f, 0.0f, 0.0f));
	Meteor->InitMeteor(Data, FVector::ZeroVector, 500.0f);
	Meteor->Tick(0.0f);
	TestTrue(TEXT("Distant approaching meteor remains active"), Meteor->IsMeteorActive());
	Meteor->SetActorLocation(FVector(600.0f, 0.0f, 0.0f));
	Meteor->Tick(0.0f);
	TestFalse(TEXT("Passed meteor returns to pool"), Meteor->IsMeteorActive());
	World->DestroyWorld(false);
	return true;
}
#endif
