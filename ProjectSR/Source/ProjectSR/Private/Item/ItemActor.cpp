// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemActor.h"
#include "Components/SphereComponent.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision_ = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision_->InitSphereRadius(100.0f);
    SphereCollision_->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereCollision_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SetRootComponent(SphereCollision_);
}

void AItemActor::InitializeItemActor(const UItemDataAsset* InItemData)
{
    ItemData_ = InItemData;
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

