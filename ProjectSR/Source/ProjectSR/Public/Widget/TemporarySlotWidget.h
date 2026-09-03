// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TemporarySlotWidget.generated.h"

class UImage;
class UTextBlock;
struct FInventorySlot;

UCLASS()
class PROJECTSR_API UTemporarySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeSlot(const FInventorySlot* InSlot);
    void SetVisual(UTexture2D* InIcon, int32 InCount);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> IconImage = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CountText = nullptr;

};
