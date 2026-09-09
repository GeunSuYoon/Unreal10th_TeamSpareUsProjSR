// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/OxygenTankItemAction.h"
#include "Component/StatComponent.h"
#include "Interface/StatComponentInterface.h"

void UOxygenTankItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    if (InTarget->GetClass()->ImplementsInterface(UStatComponentInterface::StaticClass()))
    {
        IStatComponentInterface::Execute_RecoverOxigen(InTarget, OxygenAmount);
    }
}
