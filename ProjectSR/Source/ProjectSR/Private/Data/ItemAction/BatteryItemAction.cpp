// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/BatteryItemAction.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "SpaceShip/SpaceShipActor.h"

void UBatteryItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBatteryItemAction::ExecuteItemAction_Implementation()] : World가 nullptr입니다."));
        return;
    }

    USpaceSalvageWorldSubsystem* Subsystem = World->GetSubsystem<USpaceSalvageWorldSubsystem>();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBatteryItemAction::ExecuteItemAction_Implementation()] : SpaceSalvageWorldSubsystem이 nullptr입니다."));
        return;
    }

    ASpaceShipActor* SpaceShip = Subsystem->GetSpaceShipActor();
    if (!SpaceShip)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UBatteryItemAction::ExecuteItemAction_Implementation()] : SpaceShipActor이 nullptr입니다."));
        return;
    }

    SpaceShip->GainEnergy(EnergyAmount);
}
