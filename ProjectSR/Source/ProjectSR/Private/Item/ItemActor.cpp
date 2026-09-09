// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemActor.h"
#include "Interface/InventoryComponentInterface.h"
#include "Component/InventoryComponent.h"
#include "CommonHeader/InventoryCommandTypes.h"
#include "Data/Item/ItemDataAsset.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false; // 평소엔 꺼둠, OnFocused에서 켬

    SphereCollision_ = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision_->InitSphereRadius(100.0f);
    SphereCollision_->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereCollision_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SetRootComponent(SphereCollision_);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(GetRootComponent());
    Mesh->SetCollisionProfileName("NoCollision");

    InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidgetComponent->SetupAttachment(RootComponent);
    InteractionWidgetComponent->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
    InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    InteractionWidgetComponent->SetVisibility(false);
    InteractionWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

    if (Result.bSuccess && Result.RemainingCount == 0)
    {
        FinishUsingPoolable();
    }
}

void AItemActor::OnFocused_Implementation(AActor* InTarget)
{
    if (!InteractionWidgetComponent)
    {
        return;
    }

    InteractionWidgetComponent->SetVisibility(true);
    SetActorTickEnabled(true);
    UpdateInteractionWidgetFacing__();
}

void AItemActor::OnUnfocused_Implementation(AActor* InTarget)
{
    if (!InteractionWidgetComponent)
    {
        return;
    }

    InteractionWidgetComponent->SetVisibility(false);
    SetActorTickEnabled(false);
}

void AItemActor::SetRelativeVelocity(const FVector& InVelocity)
{
	this->RelativeVelocity__ = InVelocity;
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

	this->GetRootComponent()->AddRelativeLocation(this->RelativeVelocity__ * DeltaTime);

    UpdateInteractionWidgetFacing__();
}

void AItemActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}

void AItemActor::UpdateInteractionWidgetFacing__()
{
    if (!InteractionWidgetComponent)
    {
        return;
    }

    APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
    if (!CamManager)
    {
        return;
    }

    const FVector CameraLoc = CamManager->GetCameraLocation();
    const FVector ObjectLoc = GetActorLocation();

    const FVector Dir = (CameraLoc - ObjectLoc).GetSafeNormal();
    const FVector NewWidgetLoc = ObjectLoc + Dir * WidgetOffsetDistance__;

    InteractionWidgetComponent->SetWorldLocation(NewWidgetLoc);
    InteractionWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(NewWidgetLoc, CameraLoc));
}

