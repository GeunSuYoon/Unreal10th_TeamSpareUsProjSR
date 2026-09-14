// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Widget/MainPanel/Upgrade/ShipUpgradeDetailUserWidget.h"
#include "LazerUpgradeUserWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API ULazerUpgradeUserWidget : public UShipUpgradeDetailUserWidget
{
	GENERATED_BODY()

public:
	void	BindToDataAsset();

protected:
    virtual EUpgradeTarget GetUpgradeTarget() const override { return EUpgradeTarget::Lazer; }
    virtual void RefreshStats(const FShipUpgradePreview& Preview) override;
	int32	NowLevel_ = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextLevel = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentDurability = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextDurability = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentReactiveEnergy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextReactiveEnergy = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentOperationalEnergy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextOperationalEnergy = nullptr;

};
