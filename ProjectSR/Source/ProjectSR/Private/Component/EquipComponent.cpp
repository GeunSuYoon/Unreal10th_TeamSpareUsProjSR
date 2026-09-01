// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UEquipComponent::UEquipComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UEquipComponent::EquipItem(EEquipmentSlot Slot, TSubclassOf<AActor> EquipActorClass)
{
	// 비었거나 액터가 없으면 false
	if (Slot == EEquipmentSlot::None || !EquipActorClass) return false;

	// 이미 동일 슬롯에 다른 장비가 있다면 해제
	if (EquippedActors.Contains(Slot) && EquippedActors[Slot] != nullptr)
	{
		UnEquipItem(Slot);
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return false;

	// 장비 액터 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = CharacterOwner;
	SpawnParams.Instigator = CharacterOwner;

	AActor* NewEquipActor = GetWorld()->SpawnActor<AActor>(EquipActorClass, SpawnParams);
	if (!NewEquipActor) return false;

	// 슬롯에 따른 소켓 이름 지정
	FName SocketName = NAME_None;
	switch (Slot)
	{
	case EEquipmentSlot::SpaceSuit:		SocketName = TEXT("Body_Socket"); break;
	case EEquipmentSlot::DragMachine:	SocketName = TEXT("RightHand_Socket"); break;
	case EEquipmentSlot::Weapon:		SocketName = TEXT("LeftHand_Socket"); break;
	default: break;
	}

	// 캐릭터 메시 소켓에 부착
	if (SocketName != NAME_None)
	{
		NewEquipActor->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}

	// 맵에 저장 및 이벤트 방송
	EquippedActors.Add(Slot, NewEquipActor);
	OnEquipmentChanged.Broadcast(Slot, NewEquipActor);

	return true;
}

bool UEquipComponent::UnEquipItem(EEquipmentSlot Slot)
{
	if (!EquippedActors.Contains(Slot) || EquippedActors[Slot] == nullptr) return false;

	// 액터 파괴 및 맵에서 제거
	EquippedActors[Slot]->Destroy();
	EquippedActors.Add(Slot, nullptr);

	OnEquipmentChanged.Broadcast(Slot, nullptr);
	return true;
}

AActor* UEquipComponent::GetEquippedActor(EEquipmentSlot Slot) const
{
	if (EquippedActors.Contains(Slot))
	{
		return EquippedActors[Slot];
	}
	return nullptr;
}


// Called when the game starts
void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

