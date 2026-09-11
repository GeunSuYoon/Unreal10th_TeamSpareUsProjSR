// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UUserWidget;

/**
 * Game mode used only by the boot/main-menu map.
 *
 * It owns the menu lifetime and input mode so the menu blueprint only needs to
 * contain the visual widgets with the expected names.
 */
UCLASS()
class PROJECTSR_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UUserWidget> InstructionsWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UUserWidget> SettingsWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	FName GameplayLevel = TEXT("/Game/Blueprint/GeunSuYoon/TestGame/Lv04_TestFinal");

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MainMenuWidget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> OpenedSubMenuWidget;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleInstructionsClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleExitClicked();

	UFUNCTION()
	void HandleBackClicked();

	void OpenSubMenu(TSubclassOf<UUserWidget> WidgetClass, FName BackButtonName);
	void ApplyMenuInputMode(UUserWidget* WidgetToFocus) const;
};
