// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResultUserWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_DELEGATE(FOnGameResultMainMenu);

UCLASS()
class PROJECTSR_API UGameResultUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetResult(bool bGameCleared, const FText& InDescription);

	FOnGameResultMainMenu OnGoToMainMenu;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Description = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> GoToMainButton = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Result")
	FText GameOverTitleText = NSLOCTEXT("GameResult", "GameOverTitle", "Game Over");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Result")
	FText GameClearTitleText = NSLOCTEXT("GameResult", "GameClearTitle", "Game Clear");

private:
	UFUNCTION()
	void HandleGoToMainClicked__();
};
