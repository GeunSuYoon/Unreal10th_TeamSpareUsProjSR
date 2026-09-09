// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/PlateItemAction.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "Interface/DurabilityInterface.h"
#include "SpaceShip/SpaceShipActor.h"

void UPlateItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlateItemAction::ExecuteItemAction_Implementation()] : World가 nullptr입니다."));
        return;
    }

    USpaceSalvageWorldSubsystem* Subsystem = World->GetSubsystem<USpaceSalvageWorldSubsystem>();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlateItemAction::ExecuteItemAction_Implementation()] : SpaceSalvageWorldSubsystem이 nullptr입니다."));
        return;
    }

    ASpaceShipActor* SpaceShip = Subsystem->GetSpaceShipActor();
    if (!SpaceShip)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlateItemAction::ExecuteItemAction_Implementation()] : SpaceShipActor이 nullptr입니다."));
        return;
    }

    if (!SpaceShip->GetClass()->ImplementsInterface(UDurabilityInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UPlateItemAction::ExecuteItemAction_Implementation()] : SpaceShipActor이 IDurabilityInterface를 구현하지 않았습니다."));
        return;
    }

    IDurabilityInterface::Execute_RepairDurability(SpaceShip, DurabilityAmount);
}
