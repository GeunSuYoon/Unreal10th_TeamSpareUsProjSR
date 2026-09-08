// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipUpgradeSelectUserWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMainPanelUpgradeHomeSelect, EUpgradeMenuPage, InPage);

/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipUpgradeSelectUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FOnMainPanelUpgradeHomeSelect	OnMainPanelUpgradeHomeSelect;

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void	OnSpaceShipButtonClick();
	UFUNCTION()
	void	OnLazerButtonClick();
	UFUNCTION()
	void	OnMachineArmButtonClick();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	SpaceShipButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	LazerButton = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	MachineArmButton = nullptr;

private:
	EUpgradeMenuPage	MyPage__ = EUpgradeMenuPage::Home;
};
