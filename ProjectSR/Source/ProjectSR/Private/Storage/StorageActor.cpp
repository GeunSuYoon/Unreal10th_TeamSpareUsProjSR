// Fill out your copyright notice in the Description page of Project Settings.


#include "Storage/StorageActor.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryComponentInterface.h"
#include "Widget/SRMainHUD.h"

AStorageActor::AStorageActor()
{
    PrimaryActorTick.bCanEverTick = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AStorageActor::Interact_Implementation(AActor* InTarget)
{
    if (!InTarget->GetClass()->ImplementsInterface(UInventoryComponentInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("[AStorageActor::Interact_Implementation()] : 캐릭터가 IInventoryComponentInterface를 구현하지 않음."));
        return;
    }

    if (!bRegistered)
    {
        if (ASRMainHUD* Hud = Cast<ASRMainHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
        {
            Hud->RegisterStorage(this);
            bRegistered = true;
        }
    }

    OnInteractStorage.ExecuteIfBound();
}

UInventoryComponent* AStorageActor::GetInventoryComponent_Implementation()
{
    return InventoryComponent;
}

void AStorageActor::BeginPlay()
{
    Super::BeginPlay();
}

void AStorageActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

