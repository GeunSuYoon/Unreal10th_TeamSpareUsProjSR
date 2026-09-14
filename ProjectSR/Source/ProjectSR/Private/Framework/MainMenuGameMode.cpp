// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/MainMenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Save/SurvivalSaveSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogMainMenu, Log, All);

AMainMenuGameMode::AMainMenuGameMode()
{
	// The boot map is only a background for UMG; it must not spawn a playable pawn.
	DefaultPawnClass = nullptr;

	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuClassFinder(
		TEXT("/Game/Blueprint/GeunSuYoon/Widget/StartWidget/WBP_MainMenu"));
	if (MainMenuClassFinder.Succeeded())
	{
		MainMenuWidgetClass = MainMenuClassFinder.Class;
	}

}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController) || !MainMenuWidgetClass)
	{
		UE_LOG(LogMainMenu, Error, TEXT("Cannot open the main menu: PlayerController or MainMenuWidgetClass is invalid."));
		return;
	}

	// A game-over screen leaves the local player in UI-only input. Clear any
	// carried pause/input-ignore state before this menu takes ownership of input.
	UGameplayStatics::SetGamePaused(this, false);
	PlayerController->ResetIgnoreInputFlags();
	PlayerController->FlushPressedKeys();

	MainMenuWidget = CreateWidget<UUserWidget>(PlayerController, MainMenuWidgetClass);
	if (!IsValid(MainMenuWidget))
	{
		UE_LOG(LogMainMenu, Error, TEXT("Failed to create WBP_MainMenu."));
		return;
	}

	MainMenuWidget->AddToViewport(100);

	auto FindButton = [this](FName WidgetName) -> UButton*
	{
		UButton* Button = Cast<UButton>(MainMenuWidget->GetWidgetFromName(WidgetName));
		if (!Button)
		{
			UE_LOG(LogMainMenu, Error, TEXT("WBP_MainMenu requires a Button named '%s'."), *WidgetName.ToString());
		}
		return Button;
	};

	UButton* StartButton = FindButton(TEXT("Menu_01Start"));
	if (StartButton)
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleStartClicked);
	}
	if (UWidget* InstructionsButton = MainMenuWidget->GetWidgetFromName(TEXT("Menu_02Instructions")))
	{
		InstructionsButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (UWidget* SettingsButton = MainMenuWidget->GetWidgetFromName(TEXT("Menu_03Settings")))
	{
		SettingsButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (UButton* ExitButton = FindButton(TEXT("Menu_04Exit")))
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleExitClicked);
	}

	ApplyMenuInputMode(StartButton);
}

void AMainMenuGameMode::HandleStartClicked()
{
	StartOrContinueGame();
}

void AMainMenuGameMode::StartOrContinueGame()
{
	if (GameplayLevel.IsNone())
	{
		UE_LOG(LogMainMenu, Error, TEXT("GameplayLevel is not configured."));
		return;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USurvivalSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<USurvivalSaveSubsystem>();
			SaveSubsystem && SaveSubsystem->HasSaveGame())
		{
			SaveSubsystem->RequestLoad();
		}
	}
	UGameplayStatics::OpenLevel(this, GameplayLevel);
}

void AMainMenuGameMode::HandleExitClicked()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
	}
}

void AMainMenuGameMode::ApplyMenuInputMode(UWidget* WidgetToFocus) const
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		if (IsValid(WidgetToFocus))
		{
			InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		}

		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
}
