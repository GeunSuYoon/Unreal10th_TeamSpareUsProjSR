#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "SpaceShip/LazerComponent.h"
// #include "SpaceShip/MachineArmComponent.h" // MachineArm feature retired.

namespace UpgradeTests
{
    FIngredient Ingredient(UItemDataAsset* Item, int32 Count)
    {
        FIngredient Result;
        Result.ItemData = Item;
        Result.Quantity = Count;
        return Result;
    }
    void SetTable(UObject* Object, FName PropertyName, UDataTable* Table)
    {
        FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Object->GetClass(), PropertyName);
        check(Property);
        Property->SetObjectPropertyValue_InContainer(Object, Table);
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUpgradeInventoryTest, "ProjectSR.Upgrade.AtomicIngredients",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUpgradeInventoryTest::RunTest(const FString& Parameters)
{
    auto* Item = NewObject<UItemDataAsset>();
    Item->MaxStackCount = 100;
    auto* MissingItem = NewObject<UItemDataAsset>();
    auto* First = NewObject<UInventoryComponent>();
    auto* Second = NewObject<UInventoryComponent>();
    First->SetSlot(0, Item, 3);
    Second->SetSlot(0, Item, 4);
    TArray<UInventoryComponent*> Inventories { First, Second, First };
    TArray<FIngredient> Ingredients { UpgradeTests::Ingredient(Item, 4), UpgradeTests::Ingredient(Item, 4) };
    TestFalse(TEXT("Duplicate sources cannot pay twice; duplicate requirements are aggregated"),
        UInventoryComponent::ProcessIngredients(Ingredients, Inventories, true));
    TestEqual(TEXT("Failed consume preserves first inventory"), First->GetTotalItemCount(Item), 3);
    TestEqual(TEXT("Failed consume preserves second inventory"), Second->GetTotalItemCount(Item), 4);
    Ingredients = { UpgradeTests::Ingredient(Item, 2), UpgradeTests::Ingredient(MissingItem, 1) };
    TestFalse(TEXT("Missing later ingredient aborts entire operation"),
        UInventoryComponent::ProcessIngredients(Ingredients, Inventories, true));
    TestEqual(TEXT("No partial consumption"), First->GetTotalItemCount(Item), 3);
    First->GetSlot(0)->bDragging = true;
    Ingredients = { UpgradeTests::Ingredient(Item, 5) };
    TestFalse(TEXT("Dragging items excluded"), UInventoryComponent::ProcessIngredients(Ingredients, Inventories, false));
    First->GetSlot(0)->bDragging = false;
    Ingredients = { UpgradeTests::Ingredient(Item, 3), UpgradeTests::Ingredient(Item, 4) };
    TestTrue(TEXT("Preview can pay"), UInventoryComponent::ProcessIngredients(Ingredients, Inventories, false));
    TestEqual(TEXT("Preview never consumes"), First->GetTotalItemCount(Item), 3);
    TestTrue(TEXT("Spend across unique inventories"), UInventoryComponent::ProcessIngredients(Ingredients, Inventories, true));
    TestEqual(TEXT("First exhausted"), First->GetTotalItemCount(Item), 0);
    TestEqual(TEXT("Second exhausted"), Second->GetTotalItemCount(Item), 0);
    Ingredients = { UpgradeTests::Ingredient(Item, 0) };
    TestFalse(TEXT("Zero quantity rejected"), UInventoryComponent::ProcessIngredients(Ingredients, Inventories, true));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShipUpgradeFlowTest, "ProjectSR.Upgrade.Progression",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShipUpgradeFlowTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("Test world"), World)) return false;
    ASpaceShipActor* Ship = World->SpawnActor<ASpaceShipActor>();
    if (!Ship) { World->DestroyWorld(false); return false; }
    auto* Upgrade = Ship->GetUpgradeComponent();
    auto* Inventory = NewObject<UInventoryComponent>();
    auto* Item = NewObject<UItemDataAsset>();
    Item->MaxStackCount = 100;
    Inventory->SetSlot(0, Item, 10);
    Upgrade->SetInventories({ Inventory });
    FSpaceShipStat Initial;
    Initial.Level = 1;
    Initial.MaxDurability = 100;
    Initial.MaxEnergy = 100;
    Ship->ApplySpaceShipStat(Initial);
    auto* ShipTable = NewObject<UDataTable>();
    ShipTable->RowStruct = FSpaceShipUpgrade::StaticStruct();
    FSpaceShipUpgrade ShipRow;
    ShipRow.Level = 2;
    ShipRow.SpaceShipStat.MaxDurability = 250;
    ShipRow.SpaceShipStat.MaxEnergy = 300;
    ShipRow.SpaceShipStat.MaxCapacity = 500;
    ShipRow.Ingredients = { UpgradeTests::Ingredient(Item, 3) };
    ShipRow.UnlockRecipeIds = { FName(TEXT("UnlockedByShip")) };
    ShipTable->AddRow(TEXT("AnyRowName"), ShipRow);
    UpgradeTests::SetTable(Upgrade, TEXT("SpaceShipUpgradeTable"), ShipTable);
    auto* LazerTable = NewObject<UDataTable>();
    LazerTable->RowStruct = FLazerUpgrade::StaticStruct();
    FLazerUpgrade LazerRow;
    LazerRow.Level = 1;
    LazerRow.LazerStat.Damage = 20;
    LazerRow.Ingredients = { UpgradeTests::Ingredient(Item, 2) };
    LazerTable->AddRow(TEXT("One"), LazerRow);
    LazerRow.Level = 2;
    LazerTable->AddRow(TEXT("Two"), LazerRow);
    UpgradeTests::SetTable(Upgrade, TEXT("LazerUpgradeTable"), LazerTable);
    TestEqual(TEXT("Lazer target equal to ship is blocked"), Upgrade->TryUpgrade(EUpgradeTarget::Lazer),
        EUpgradeResult::RequiresHigherShipLevel);
    TestEqual(TEXT("Blocked lazer spends nothing"), Inventory->GetTotalItemCount(Item), 10);
    TestEqual(TEXT("Missing crafting connection blocks before consumption"), Upgrade->TryUpgrade(EUpgradeTarget::SpaceShip),
        EUpgradeResult::InvalidData);
    TestEqual(TEXT("Invalid recipe connection spends nothing"), Inventory->GetTotalItemCount(Item), 10);
    auto* Crafting = NewObject<UCraftingComponent>();
    auto* RecipeTable = NewObject<UDataTable>();
    RecipeTable->RowStruct = FRecipeTableRow::StaticStruct();
    RecipeTable->AddRow(TEXT("UnlockedByShip"), FRecipeTableRow());
    UpgradeTests::SetTable(Crafting, TEXT("RecipeTable_"), RecipeTable);
    Upgrade->SetCraftingComponent(Crafting);
    Ship->UseEnergy(50);
    TestEqual(TEXT("Ship upgrade succeeds"), Upgrade->TryUpgrade(EUpgradeTarget::SpaceShip), EUpgradeResult::Success);
    TestEqual(TEXT("Target level overrides nested default"), Ship->GetStat().Level, 2);
    TestEqual(TEXT("Energy refilled"), Ship->GetCurrentEnergy(), 300.f);
    TestEqual(TEXT("Durability refilled"), Ship->GetCurrentDurability(), 250.f);
    TestEqual(TEXT("Capacity applied"), Ship->GetStat().MaxCapacity, 500.f);
    TestTrue(TEXT("Recipe unlocked"), Crafting->IsUnlockedRecipe(TEXT("UnlockedByShip"), true));
    TestEqual(TEXT("Lazer now allowed"), Upgrade->TryUpgrade(EUpgradeTarget::Lazer), EUpgradeResult::Success);
    TestEqual(TEXT("Lazer must remain strictly lower"), Upgrade->TryUpgrade(EUpgradeTarget::Lazer),
        EUpgradeResult::RequiresHigherShipLevel);
    TestEqual(TEXT("Only successful upgrades charged"), Inventory->GetTotalItemCount(Item), 5);
    TestEqual(TEXT("Final ship level recognized"), Upgrade->TryUpgrade(EUpgradeTarget::SpaceShip), EUpgradeResult::MaxLevel);
    ShipRow.Level = 4;
    ShipTable->AddRow(TEXT("Gap"), ShipRow);
    TestEqual(TEXT("Missing intermediate level is invalid, not max"), Upgrade->GetPreview(EUpgradeTarget::SpaceShip).Result,
        EUpgradeResult::InvalidData);
    ShipTable->RemoveRow(TEXT("Gap"));
    ShipRow.Level = 2;
    ShipTable->AddRow(TEXT("DuplicateLevel"), ShipRow);
    TestEqual(TEXT("Duplicate target levels are invalid"), Upgrade->GetPreview(EUpgradeTarget::SpaceShip).Result,
        EUpgradeResult::InvalidData);
    ShipTable->RemoveRow(TEXT("DuplicateLevel"));
	// MachineArm feature retired.
	/*
	auto* ArmTable = NewObject<UDataTable>();
	ArmTable->RowStruct = FMachineArmUpgrade::StaticStruct();
	FMachineArmUpgrade ArmRow;
	ArmRow.Level = 1;
	ArmRow.MachineArmStat.ItemCollectTime = 2.f;
	ArmRow.MachineArmStat.ItemCollectWeight = 10.f;
	ArmTable->AddRow(TEXT("Arm"), ArmRow);
	UpgradeTests::SetTable(Upgrade, TEXT("MachineArmUpgradeTable"), ArmTable);
	TestEqual(TEXT("Machine arm upgrades independently"), Upgrade->TryUpgrade(EUpgradeTarget::MachineArm), EUpgradeResult::Success);
	TestEqual(TEXT("Machine arm time applied"), Ship->GetMachineArmComponent()->GetItemCollectTime(), 2.f);
	*/
    TestTrue(TEXT("Restore is idempotent"), Upgrade->RestoreRecipeUnlocks());
    TestEqual(TEXT("Restore does not spend resources"), Inventory->GetTotalItemCount(Item), 5);
    World->DestroyWorld(false);
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShipLevelZeroTest, "ProjectSR.Upgrade.LevelZeroInitialization",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FShipLevelZeroTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("Test world"), World)) return false;
    ASpaceShipActor* Ship = World->SpawnActor<ASpaceShipActor>();
    if (!Ship) { World->DestroyWorld(false); return false; }
    auto* Upgrade = Ship->GetUpgradeComponent();
    TestFalse(TEXT("Missing table can be retried later"), Upgrade->InitializeLevelZero());
    auto* Table = NewObject<UDataTable>();
    Table->RowStruct = FSpaceShipUpgrade::StaticStruct();
    FSpaceShipUpgrade Row;
    Row.Level = 0;
    Row.SpaceShipStat.Level = 99;
    Row.SpaceShipStat.MaxDurability = 100.f;
    Row.SpaceShipStat.MaxEnergy = 80.f;
    auto* Item = NewObject<UItemDataAsset>();
    Row.Ingredients = { UpgradeTests::Ingredient(Item, 100) };
    Table->AddRow(TEXT("Initial"), Row);
    Table->AddRow(TEXT("Duplicate"), Row);
    UpgradeTests::SetTable(Upgrade, TEXT("SpaceShipUpgradeTable"), Table);
    TestFalse(TEXT("Duplicate zero rows rejected"), Upgrade->InitializeLevelZero());
    Table->RemoveRow(TEXT("Duplicate"));
    TestTrue(TEXT("Initialize without paying ingredients"), Upgrade->InitializeLevelZero());
    TestEqual(TEXT("Outer row level used"), Ship->GetStat().Level, 0);
    TestEqual(TEXT("Initial durability filled"), Ship->GetCurrentDurability(), 100.f);
    Ship->UseEnergy(10.f);
    TestTrue(TEXT("Repeated initialization succeeds without changes"), Upgrade->InitializeLevelZero());
    TestEqual(TEXT("Repeated initialization does not refill energy"), Ship->GetCurrentEnergy(), 70.f);
    World->DestroyWorld(false);
    return true;
}
#endif
