// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/OpenableWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "PlayMenuUserWidget.generated.h"

class UButton;

UCLASS()
class PROJECTSR_API UPlayMenuUserWidget : public UUserWidget, public IOpenableWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void OpenSelfWidget_Implementation() override;
	virtual void CloseSelfWidget_Implementation() override;

	FOnWidgetOpen OnWidgetOpen;
	FOnWidgetClose OnWidgetClose;

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Menu_01Start = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Menu_04Exit = nullptr;

private:
	UFUNCTION()
	void HandleResumeClicked__();

	UFUNCTION()
	void HandleExitClicked__();
};
