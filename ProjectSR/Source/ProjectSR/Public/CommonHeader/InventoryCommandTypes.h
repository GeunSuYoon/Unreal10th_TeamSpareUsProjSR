// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryCommandTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryCommandType : uint8
{
    None,
    Add,
    Move,
    Drop,
    Use,
    Clear,
    Equip
};

USTRUCT(BlueprintType)
struct PROJECTSR_API FInventoryCommand
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    EInventoryCommandType Type = EInventoryCommandType::None;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 Count = 1;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    TObjectPtr<const UItemDataAsset> ItemData = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 SourceIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 TargetIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    FVector DropLocation = FVector::ZeroVector;

public:
    static FInventoryCommand MakeAddCommand(const UItemDataAsset* InItemData, int32 InCount = 1)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Add;
        Command.ItemData = InItemData;
        Command.Count = InCount;

        return Command;
    }

    static FInventoryCommand MakeMoveCommand(int32 InSourceIndex, int32 InTargetIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Move;
        Command.SourceIndex = InSourceIndex;
        Command.TargetIndex = InTargetIndex;

        return Command;
    }

    static FInventoryCommand MakeDropCommand(int32 InSlotIndex, const FVector& InDropLocation)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Drop;
        Command.TargetIndex = InSlotIndex;
        Command.DropLocation = InDropLocation;

        return Command;
    }

    static FInventoryCommand MakeUseCommand(int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Use;
        Command.TargetIndex = InSlotIndex;

        return Command;
    }

    static FInventoryCommand MakeClearCommand(int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Clear;
        Command.TargetIndex = InSlotIndex;

        return Command;
    }

    static FInventoryCommand MakeEquipCommand(int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Equip;
        Command.TargetIndex = InSlotIndex;

        return Command;
    }

};

USTRUCT(BlueprintType)
struct PROJECTSR_API FInventoryCommandResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    bool bSuccess = false;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingCount = 0;

};
