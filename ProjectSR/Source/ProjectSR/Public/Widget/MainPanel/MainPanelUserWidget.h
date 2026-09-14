// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/MainPanelEnum.h"
#include "Interface/OpenableWidgetInterface.h"
#include "Interface/WidgetStackHostInterface.h"

#include "Blueprint/UserWidget.h"
#include "MainPanelUserWidget.generated.h"

class UButton;
class UWidgetSwitcher;

class UMainPanelHomeUserWidget;
class USpaceShipActorStatusUserWidget;
class UItemManagerWidget;
class UMeteorEventUserWidget;
class USpaceShipUpgaradeMainUserWidget;
class URecipeListWidget;
class ASpaceShipActor;

/**
 * 
 */
UCLASS()
class PROJECTSR_API UMainPanelUserWidget : public UUserWidget, public IOpenableWidgetInterface, public IWidgetStackHostInterface
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	bool	HandleMeteorMoveKey(const FKeyEvent& InKeyEvent);

	virtual void	OpenSelfWidget_Implementation() override;
	virtual void	CloseSelfWidget_Implementation() override;

	virtual bool	CloseTopWidget_Implementation() override;
	virtual void	ClearStackWidget_Implementation() override;

	UFUNCTION()
	void	OpenMainPanel();

	//UFUNCTION(BlueprintCallable)
	void	SwitchWidget(int32 InIndex);

	UFUNCTION()
	void	SwitchTargetWidget(EMainPanelMenuPage InPage);

	UFUNCTION()
	void	CloseDetect();
	UFUNCTION()
	void	BackspaceDetect();

	FOnWidgetOpen	OnWidgetOpen;
	FOnWidgetClose	OnWidgetClose;

	UItemManagerWidget*	GetWarehouseWidget() { return (this->Warehouse); }
	URecipeListWidget*	GeCraftingRecipeListWidget() { return (this->CraftingRecipeList); }

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	BackSpaceButton = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton>	CloseButton = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher>					MainPanelSwitcher = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainPanelHomeUserWidget>		MainPanelHome = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipActorStatusUserWidget>	SpaceShipStatus = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UItemManagerWidget>				Warehouse = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMeteorEventUserWidget>			MeteoEvent = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgaradeMainUserWidget>SpaceShipUpgrade = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<URecipeListWidget>				CraftingRecipeList = nullptr;

private:
	TArray<TObjectPtr<UWidget>>	OpenWidgetStack__;
	//int32						StackSize__ = 0;

};
