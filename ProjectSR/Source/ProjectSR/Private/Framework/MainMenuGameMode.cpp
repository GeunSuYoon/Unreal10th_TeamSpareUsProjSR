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

	static ConstructorHelpers::FClassFinder<UUserWidget> InstructionsClassFinder(
		TEXT("/Game/Blueprint/GeunSuYoon/Widget/StartWidget/WBP_MainMenu_02Instructions"));
	if (InstructionsClassFinder.Succeeded())
	{
		InstructionsWidgetClass = InstructionsClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SettingsClassFinder(
		TEXT("/Game/Blueprint/GeunSuYoon/Widget/StartWidget/WBP_MainMenu_03Settings"));
	if (SettingsClassFinder.Succeeded())
	{
		SettingsWidgetClass = SettingsClassFinder.Class;
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

	if (UButton* StartButton = FindButton(TEXT("Menu_01Start")))
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleStartClicked);
	}
	if (UButton* InstructionsButton = FindButton(TEXT("Menu_02Instructions")))
	{
		InstructionsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleInstructionsClicked);
	}
	if (UButton* SettingsButton = FindButton(TEXT("Menu_03Settings")))
	{
		SettingsButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSettingsClicked);
	}
	if (UButton* ExitButton = FindButton(TEXT("Menu_04Exit")))
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleExitClicked);
	}

	ApplyMenuInputMode(MainMenuWidget);
}

void AMainMenuGameMode::HandleStartClicked()
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

void AMainMenuGameMode::HandleInstructionsClicked()
{
	OpenSubMenu(InstructionsWidgetClass, TEXT("Menu_02_BackButton"));
}

void AMainMenuGameMode::HandleSettingsClicked()
{
	OpenSubMenu(SettingsWidgetClass, TEXT("Menu_03_BackButton"));
}

void AMainMenuGameMode::HandleExitClicked()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
	}
}

void AMainMenuGameMode::HandleBackClicked()
{
	if (IsValid(OpenedSubMenuWidget))
	{
		OpenedSubMenuWidget->RemoveFromParent();
		OpenedSubMenuWidget = nullptr;
	}

	if (IsValid(MainMenuWidget))
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
		ApplyMenuInputMode(MainMenuWidget);
	}
}

void AMainMenuGameMode::OpenSubMenu(TSubclassOf<UUserWidget> WidgetClass, FName BackButtonName)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController) || !WidgetClass || !IsValid(MainMenuWidget))
	{
		UE_LOG(LogMainMenu, Error, TEXT("Cannot open submenu '%s'."), *GetNameSafe(WidgetClass.Get()));
		return;
	}

	if (IsValid(OpenedSubMenuWidget))
	{
		OpenedSubMenuWidget->RemoveFromParent();
	}

	OpenedSubMenuWidget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
	if (!IsValid(OpenedSubMenuWidget))
	{
		return;
	}

	if (UButton* BackButton = Cast<UButton>(OpenedSubMenuWidget->GetWidgetFromName(BackButtonName)))
	{
		BackButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleBackClicked);
	}
	else
	{
		UE_LOG(LogMainMenu, Error, TEXT("Submenu requires a Button named '%s'."), *BackButtonName.ToString());
	}

	MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	OpenedSubMenuWidget->AddToViewport(101);
	ApplyMenuInputMode(OpenedSubMenuWidget);
}

void AMainMenuGameMode::ApplyMenuInputMode(UUserWidget* WidgetToFocus) const
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
