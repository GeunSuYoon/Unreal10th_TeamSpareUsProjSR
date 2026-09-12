// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/WidgetStackHostInterface.h"

#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

class ASpaceShipActor;
class APlayerCharacter;
class UMeteorWarningUserWidget;
class UInventoryWindowWidget;
class UMainPanelUserWidget;
class URecipeListWidget;
class UManufactureWidget;
class USpaceShipAlarmUserWidget;
class UInventoryAlarmUserWidget;
class UPlayerStatUserWidget;
class USpaceShipStatUserWidget;
class ASurvivalLoopActor;
class UDayCountUserWidget;

/**
 *
 */
UCLASS()
class PROJECTSR_API UMainUserWidget : public UUserWidget, public IWidgetStackHostInterface
{
    GENERATED_BODY()

public:
    void	BindToPlayer(APlayerCharacter* InPlayerCharacter);
    void	BindToSpaceShip(ASpaceShipActor* InSpaceShipActor);
    void	BindToCharacter(ACharacter* InCharacter);
	void	BindToSurvivalLoop(ASurvivalLoopActor* InSurvivalLoop);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryWindowWidget>		InventoryWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMeteorWarningUserWidget>	MeteoWarningWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UManufactureWidget>			CraftingManufacture;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMainPanelUserWidget>		MainPanelWidget = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<USpaceShipAlarmUserWidget>	SpaceShipAlarm = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryAlarmUserWidget>	InventoryAlarm = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPlayerStatUserWidget>		PlayerStatWidget = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<USpaceShipStatUserWidget>	SpaceShipStat = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UDayCountUserWidget>			DayCount = nullptr;

    virtual bool	CloseTopWidget_Implementation() override;
    virtual void	ClearStackWidget_Implementation() override;

protected:
    virtual void	NativeOnInitialized() override;
    virtual FReply	NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void	BindWidget__();

    void RegisterOpenWidget__(UUserWidget* Widget);
    void UnregisterOpenWidget__(UUserWidget* Widget);
    void UpdateInputMode__();

    UFUNCTION()
    void HandleMainPanelOpened__(UUserWidget* InWidget);

    UFUNCTION()
    void HandleMainPanelClosed__(UUserWidget* InWidget);

	UFUNCTION()
	void HandleDayFadeOut__();

	UFUNCTION()
	void HandleDayFadeIn__();

    UPROPERTY(Transient)
    TArray<TObjectPtr<UUserWidget>> OpenWidgetStack__;

	bool	bIsSpaceShipBind = false;
	bool	bIsPlayerBind = false;
};
