// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SRMainHUD.h"
#include "Widget/MainUserWidget.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"

void ASRMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (this->MainUserWidgetClass_)
	{
		this->MainUserWidgetInstance_ = CreateWidget<UMainUserWidget>(GetWorld(), this->MainUserWidgetClass_);
		if (this->MainUserWidgetInstance_)
		{
			this->MainUserWidgetInstance_->AddToViewport();
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[ASRMainHUD::BeginPlay] MainUserWidgetClass_가 nullptr입니다.")
		);
		return ;
	}
	//this->MainUserWidgetInstance_->AddToViewport();

	USpaceSalvageWorldSubsystem* SpaceSubsystem = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (IsValid(SpaceSubsystem))
	{
		RegisterSpaceShip(SpaceSubsystem->GetSpaceShipActor());
	}
}

void ASRMainHUD::RegisterSpaceShip(ASpaceShipActor* InSpaceShipActor)
{
	if (!InSpaceShipActor)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[ASRMainHUD::RegisterSpaceShip] InSpaceShipActor가 nullptr입니다.")
		);
		return ;
	}
	this->MainUserWidgetInstance_->BindToSpaceShip(InSpaceShipActor);
}

void ASRMainHUD::RegisterPlayerCharacter(APlayerCharacter* InPlayerCharacter)
{
    APlayerController* PC = GetOwningPlayerController();

    if (!PC)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[ASRMainHUD::BeginPlay] PlayerController가 nullptr입니다.")
        );
        return;
    }

    this->MainUserWidgetInstance_->BindToPlayer(InPlayerCharacter);
}
