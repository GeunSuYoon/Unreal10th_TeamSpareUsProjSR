// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UUserWidget;
class UWidget;

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

	// Starts a new run, or requests the existing end-of-day checkpoint before travel.
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void StartOrContinueGame();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	FName GameplayLevel = TEXT("/Game/Blueprint/GeunSuYoon/TestGame/Lv04_TestFinal");

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MainMenuWidget;

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleExitClicked();
	void ApplyMenuInputMode(UWidget* WidgetToFocus) const;
};
