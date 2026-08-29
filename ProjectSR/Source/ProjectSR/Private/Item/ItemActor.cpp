// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemActor.h"
#include "Interface/InventoryComponentInterface.h"
#include "Component/InventoryComponent.h"
#include "CommonHeader/InventoryCommandTypes.h"
#include "Data/Item/ItemDataAsset.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"

#include "Components/SphereComponent.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision_ = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision_->InitSphereRadius(100.0f);
    SphereCollision_->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereCollision_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SetRootComponent(SphereCollision_);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(GetRootComponent());
    Mesh->SetCollisionProfileName("NoCollision");
}

void AItemActor::FinishUsingPoolable()
{
    if (UObjectPoolSubsystem* ObjectPool = GetWorld()->GetSubsystem<UObjectPoolSubsystem>())
    {
        ObjectPool->ReturnToPool(this);
    }
}

void AItemActor::OnSpawnFromPool_Implementation()
{
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
    SetActorEnableCollision(true);
}

void AItemActor::OnReturnToPool_Implementation()
{
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    SetActorEnableCollision(false);
}

void AItemActor::InitializeItemActor(const UItemDataAsset* InItemData)
{
    ItemData_ = InItemData;

    if (ItemData_)
    {
        if (UStaticMesh* MeshData = ItemData_->Mesh.LoadSynchronous())
        {
            Mesh->SetStaticMesh(MeshData);
        }
    }
}

void AItemActor::Interact_Implementation(AActor* InTarget)
{
    if (!InTarget->GetClass()->ImplementsInterface(UInventoryComponentInterface::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("[AItemActor::Interact_Implementation()] : 캐릭터가 IInventoryComponentInterface를 구현하지 않음."));
        return;
    }

    UInventoryComponent* Inventory = IInventoryComponentInterface::Execute_GetInventoryComponent(InTarget);

    if (!Inventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AItemActor::Interact_Implementation()] : 캐릭터에 UInventoryComponent가 없음."));
        return;
    }

    FInventoryCommandResult Result;
    Inventory->ExecuteCommand(
        FInventoryCommand::MakeAddCommand(ItemData_, 1),
        Result
    );

    if (Result.bSuccess)
    {
        FinishUsingPoolable();
    }
}

void AItemActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    InitializeItemActor(ItemData_);
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &AItemActor::OnBeginOverlap);
}

void AItemActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AItemActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}

