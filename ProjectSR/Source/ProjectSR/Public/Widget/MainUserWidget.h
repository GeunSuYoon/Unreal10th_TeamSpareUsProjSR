// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/WidgetStackHostInterface.h"

#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

class UMeteorWarningUserWidget;
class ASpaceShipActor;
class APlayerCharacter;
class UInventoryWindowWidget;
class UMainPanelUserWidget;
class URecipeListWidget;
class UManufactureWidget;

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

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    //TObjectPtr<UInventoryWindowWidget>	InventoryWindow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMeteorWarningUserWidget>	MeteoWarningWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URecipeListWidget>	RecipeListWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UManufactureWidget>	ManufactureWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMainPanelUserWidget>	MainPanelWidget = nullptr;

    virtual bool	CloseTopWidget_Implementation() override;
    virtual void	ClearStackWidget_Implementation() override;

private:
    TArray<UUserWidget*>	OpenWidgetStack__;

};
