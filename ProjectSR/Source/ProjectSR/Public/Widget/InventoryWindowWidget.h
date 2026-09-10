// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/OpenableWidgetInterface.h"

#include "Blueprint/UserWidget.h"
#include "InventoryWindowWidget.generated.h"

class UButton;
class UTextBlock;
class UItemManagerWidget;
class UInventoryComponent;

UCLASS()
class PROJECTSR_API UInventoryWindowWidget : public UUserWidget, public IOpenableWidgetInterface
{
    GENERATED_BODY()

public:
    void BindToInventoryComponent(UInventoryComponent* InInventoryComponent);

	void	OpenWidget();
	void	CloseWidget();

	virtual void	OpenSelfWidget_Implementation() override;
	virtual void	CloseSelfWidget_Implementation() override;

    UFUNCTION(BlueprintCallable)
    void ToggleInventoryWidget();

	FOnWidgetOpen	OnWidgetOpen;
	FOnWidgetClose	OnWidgetClose;

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    UFUNCTION()
    void OnInventoryCloseButtonClicked__();

    UFUNCTION()
    void RefreshCapacityText__(int32 InSlotIndex);

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> Inventory_CloseButton;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentCapacityText;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxCapacityText;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UItemManagerWidget> ItemManagerWidget;

private:
    TWeakObjectPtr<UInventoryComponent> TargetInventory__ = nullptr;

};
