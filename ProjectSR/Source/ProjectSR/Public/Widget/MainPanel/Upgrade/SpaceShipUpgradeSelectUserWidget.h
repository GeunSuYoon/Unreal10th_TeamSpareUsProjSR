// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Blueprint/UserWidget.h"
#include "SpaceShipUpgradeSelectUserWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnButtonClick, EUpgradeMenuPage, InPage);
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipUpgradeSelectUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FOnButtonClick	OnButtonClick;

protected:
	virtual void NativeOnInitialized() override;

	void	OnSpaceShipButtonClick();
	void	OnLazerButtonClick();
	void	OnMachineArmButtonClick();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	SpaceShipButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	LazerButton = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	MachineArmButton = nullptr;
};
