// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemManagerWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UBorder;
class UUniformGridPanel;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class PROJECTSR_API UItemManagerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitializeItemManagerWidget(UInventoryComponent* InInventoryComponent);
    void ClearInventoryWidget();
    void RefreshInventoryWidget();

protected:
    void RefreshSlotWidget_(int32 InSlotIndex) const;
    void RefreshItemDetailPanel_() const;

private:
    UFUNCTION()
    void OnItemUseButtonClicked__();

    UFUNCTION()
    void OnItemDropButtonClicked__();

    inline bool IsValidIndex__(int32 InIndex) const { return 0 <= InIndex && InIndex < SlotSize__; }

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> ItemGridPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UBorder> ItemInfoPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> Iteminfo_Image;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> Iteminfo_Name;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> Iteminfo_Weight;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> Iteminfo_Count;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> Iteminfo_Description;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> Item_Use;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> Item_Drop;

private:
    static constexpr int32 InvalidIndex = -1;

    TWeakObjectPtr<UInventoryComponent> TargetInventory__ = nullptr;
    TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets__;
    int32 SelectedSlotIndex__ = InvalidIndex;
    int32 SlotSize__ = 0;

};
