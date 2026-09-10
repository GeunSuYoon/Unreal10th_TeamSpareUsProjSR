// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/BatteryItemAction.h"
#include "SpaceShip/SpaceShipActor.h"

void UBatteryItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    if (ASpaceShipActor* SpaceShip = Cast<ASpaceShipActor>(InTarget))
    {
        SpaceShip->GainEnergy(EnergyAmount);
    }
}
