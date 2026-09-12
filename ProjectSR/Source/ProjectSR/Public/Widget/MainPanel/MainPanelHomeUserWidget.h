// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Blueprint/UserWidget.h"
#include "MainPanelHomeUserWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMainPanelHomeSelect, EMainPanelMenuPage, InPage);

/**
 * 
 */
UCLASS()
class PROJECTSR_API UMainPanelHomeUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void	SpaceShipStatusSelect();
	UFUNCTION()
	void	WarehouseSelect();
	UFUNCTION()
	void	MeteorEventSelect();
	UFUNCTION()
	void	UpgradeSelect();
	UFUNCTION()
	void	ItemCraftSelect();
	UFUNCTION()
	void	FinishDay();

	FOnMainPanelHomeSelect	OnMainPanelHomeSelect;

protected:
	virtual void	NativeOnInitialized() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	SpaceShipStatusButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	WarehouseButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	MeteorEventButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	UpgradeButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	ItemCraftButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	FinishDayButton = nullptr;

private:
	EMainPanelMenuPage	MyPage__ = EMainPanelMenuPage::Home;
};
