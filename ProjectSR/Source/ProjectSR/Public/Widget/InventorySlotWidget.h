// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UInventoryComponent;
class UImage;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FOnSlotClicked, int32);
DECLARE_DELEGATE(FOnDragStarted)

UCLASS()
class PROJECTSR_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex);
    void RefreshSlot() const;

protected:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
    UFUNCTION()
    void OnSlotButtonClicked__();

public:
    FOnSlotClicked OnSlotClicked;
    FOnDragStarted OnDragStarted;

protected:
    //UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    //TObjectPtr<UButton> Item_GridButton;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> Item_Grid_Icon;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> Item_Grid_Count;

private:
    TWeakObjectPtr<UInventoryComponent> TargetInventory__ = nullptr;
    int32 Index__ = -1;
};
