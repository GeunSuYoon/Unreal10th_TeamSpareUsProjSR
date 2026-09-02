// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWindowWidget.generated.h"

class UButton;
class UTextBlock;
class UItemManagerWidget;
class UInventoryComponent;

UCLASS()
class PROJECTSR_API UInventoryWindowWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void OpenInventoryWidget();
    void CloseInventoryWidget();

    UFUNCTION(BlueprintCallable)
    void ToggleInventoryWidget();

    inline bool IsInventoryOpen() const { return GetVisibility() == ESlateVisibility::Visible; }

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    UFUNCTION()
    void OnInventoryCloseButtonClicked__();

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> Inventory_CloseButton;

    // TODO: 
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CapacityText;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> WeightText;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UItemManagerWidget> ItemManagerWidget;

private:
    TWeakObjectPtr<UInventoryComponent> TargetInventory__ = nullptr;

};
