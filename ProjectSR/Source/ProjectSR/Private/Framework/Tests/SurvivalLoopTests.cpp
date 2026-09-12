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
	Loop->DayFadeOutDuration = 0.0f;
	Loop->DayFadeInDuration = 0.0f;
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
	Loop->Tick(1000.0f);
	TestEqual(TEXT("Time alone does not advance the day"), Loop->CurrentDay, 1);
	for (int32 Day = 2; Day <= 10; ++Day)
	{
		Loop->FinishDay();
		TestTrue(TEXT("Next-day selection enters transition"), Loop->State == ESurvivalState::WaitingForMeteor);
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
	Loop->DayFadeOutDuration = 0.0f;
	Loop->DayFadeInDuration = 0.0f;
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
	Loop->Tick(1000.0f);
	TestEqual(TEXT("Day waits for the main-panel selection"), Loop->CurrentDay, 1);
	TestEqual(TEXT("First day does not charge energy"), Ship->GetCurrentEnergy(), 10.0f);
	Player->SetActorLocation(FVector(410.0f, -980.0f, 100.0f));
	TestTrue(TEXT("Closed cabin includes the airlock controls"), Loop->IsPlayerInside());
	Player->SetActorLocation(Loop->GetActorLocation());
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
	Loop->FinishDay();
	Loop->Tick(0.01f);
	Loop->Tick(0.0f);
	TestFalse(TEXT("Insufficient energy limits recovery"), Loop->bLastDailyEnergySufficient);
	TestEqual(TEXT("Partial available energy is spent"), Ship->GetCurrentEnergy(), 0.0f);
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
