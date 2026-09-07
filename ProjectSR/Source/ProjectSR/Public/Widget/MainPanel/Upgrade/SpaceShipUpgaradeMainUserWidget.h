// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipUpgaradeMainUserWidget.generated.h"

class UWidgetSwitcher;
class USpaceShipUpgradeSelectUserWidget;
class USpaceShipUpgradeUserWidget;
class UMachineArmUpgradeUserWidget;
class ULazerUpgradeUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipUpgaradeMainUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToDataTable();
	void	BindToBackSpace();

	UFUNCTION(BlueprintCallable)
	void	SwitchWidget(EUpgradeMenuPage InPage);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher>						SpaceShipUpgradeSwitcher = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgradeSelectUserWidget>	SpaceShipUpgradeSelect = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgradeUserWidget>			SpaceShipUpgrade = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMachineArmUpgradeUserWidget>		LazerUpgrade = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<ULazerUpgradeUserWidget>				MachineArmUpgrade = nullptr;

};
