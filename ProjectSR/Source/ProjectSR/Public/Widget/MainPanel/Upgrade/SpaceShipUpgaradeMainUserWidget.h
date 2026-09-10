// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"
#include "Interface/WidgetStackHostInterface.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipUpgaradeMainUserWidget.generated.h"

class UWidgetSwitcher;
class USpaceShipUpgradeComponent;
class USpaceShipUpgradeSelectUserWidget;
class USpaceShipUpgradeUserWidget;
class UMachineArmUpgradeUserWidget;
class ULazerUpgradeUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipUpgaradeMainUserWidget : public UUserWidget, public IWidgetStackHostInterface
{
	GENERATED_BODY()
	
public:
	void	BindToDataTable();
	void BindToUpgradeComponent(USpaceShipUpgradeComponent* InComponent);
	void RefreshUpgradePages();

	UFUNCTION(BlueprintCallable)
	void	SwitchWidget(EUpgradeMenuPage InPage);
	void	SwitchWidget(int32 InIndex);
	UFUNCTION()
	void	SwitchTargetWidget(EUpgradeMenuPage InPage);

	virtual bool	CloseTopWidget_Implementation() override;
	virtual void	ClearStackWidget_Implementation() override;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher>						SpaceShipUpgradeSwitcher = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgradeSelectUserWidget>	SpaceShipUpgradeSelect = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgradeUserWidget>			SpaceShipUpgrade = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<ULazerUpgradeUserWidget>				LazerUpgrade = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMachineArmUpgradeUserWidget>		MachineArmUpgrade = nullptr;

private:
	TArray<TObjectPtr<UWidget>>	OpenWidgetStack__;
	int32						StackSize__ = 0;

	EMainPanelMenuPage	MyPage__ = EMainPanelMenuPage::Upgrade;

};
