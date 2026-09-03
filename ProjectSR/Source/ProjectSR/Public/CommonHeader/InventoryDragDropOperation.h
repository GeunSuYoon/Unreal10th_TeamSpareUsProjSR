// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

class UInventoryComponent;

UCLASS()
class PROJECTSR_API UInventoryDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TWeakObjectPtr<UInventoryComponent> SourceInventory = nullptr;

    UPROPERTY(BlueprintReadonly)
    int32 SourceIndex = -1;

};
