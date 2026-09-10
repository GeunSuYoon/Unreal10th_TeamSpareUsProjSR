// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"

#include "Widget/MainPanel/Upgrade/ShipUpgradeDetailUserWidget.h"
#include "SpaceShipUpgradeUserWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTSR_API USpaceShipUpgradeUserWidget : public UShipUpgradeDetailUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToDataAsset();

protected:
    virtual EUpgradeTarget GetUpgradeTarget() const override { return EUpgradeTarget::SpaceShip; }
    virtual void RefreshStats(const FShipUpgradePreview& Preview) override;
	int32	NowLevel_ = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextLevel = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentDurability = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextDurability = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentEnergy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextEnergy = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	CurrentSize = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock>	NextSize = nullptr;
};
