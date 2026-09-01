// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ItemAction/EquipItemAction.h"
#include "Interface/EquipComponentInterface.h"
#include "Interface/StatComponentInterface.h"
#include "Component/EquipComponent.h"
#include "Component/StatComponent.h"
#include "Player/PlayerCharacter.h"

void UEquipItemAction::ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
	AActor* TargetActor = InTarget ? InTarget : InInstigator;
	if (!TargetActor || !EquipmentData) return;

	// EquipComponent를 통한 액터/메시 장착
	if (TargetActor->GetClass()->ImplementsInterface(UEquipComponentInterface::StaticClass()))
	{
		UEquipComponent* EquipComp = IEquipComponentInterface::Execute_GetEquipComponent(TargetActor);
		if (EquipComp)
		{
			EquipComp->EquipItem(EquipmentData->EquipmentSlot, EquipmentData->ItemActorClass.Get());
		}
	}

	// StatComponent를 통한 스탯 보너스 재계산 적용
	if (TargetActor->GetClass()->ImplementsInterface(UStatComponentInterface::StaticClass()))
	{
		UStatComponent* StatComp = IStatComponentInterface::Execute_GetStatComponent(TargetActor);
		if (StatComp)
		{
			StatComp->RecalculateMaxStats(EquipmentData->StatModifiers);
		}
	}

	// 재계산된 속도를 CharacterMovement에 최종 반영 (APlayerCharacter)
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(TargetActor))
	{
		PlayerChar->RefreshMovementSpeed();
	}
}
