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
// 인벤토리 위젯 클래스 가져와야해용
class UMeteorEventUserWidget;
class USpaceShipUpgaradeMainUserWidget;
// 아이템 제작 위젯 클래스 가져와야해용
class ASpaceShipActor;

//DECLARE_DYNAMIC_DELEGATE(FOnOpenMainPanel);
//DECLARE_DYNAMIC_DELEGATE(FOnCloseMainPanel);

/**
 * 
 */
UCLASS()
class PROJECTSR_API UMainPanelUserWidget : public UUserWidget, public IOpenableWidgetInterface, public IWidgetStackHostInterface
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	virtual void	OpenSelfWidget_Implementation() override;
	virtual void	CloseSelfWidget_Implementation() override;

	virtual bool	CloseTopWidget_Implementation() override;
	virtual void	ClearStackWidget_Implementation() override;

	UFUNCTION()
	void	OpenMainPanel();

	UFUNCTION(BlueprintCallable)
	void	SwitchWidget(EMainPanelMenuPage InPage);

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
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	// TObjectPtr<UMainPanelHomeUserWidget>	MainPanelSwitcher = nullptr; // 창고자리
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMeteorEventUserWidget>			MeteoEvent = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USpaceShipUpgaradeMainUserWidget>SpaceShipUpgrade = nullptr;
	//UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	//TObjectPtr<UWidgetSwitcher>	MainPanelSwitcher = nullptr; // 제작 자리

private:
	TArray<TObjectPtr<UWidget>>	OpenWidgetStack__;
	//int32						StackSize__ = 0;

};
