// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/PlateItemAction.h"
#include "Interface/DurabilityInterface.h"

void UPlateItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    if (InTarget->GetClass()->ImplementsInterface(UDurabilityInterface::StaticClass()))
    {
        IDurabilityInterface::Execute_RepairDurability(InTarget, DurabilityAmount);
    }
}
