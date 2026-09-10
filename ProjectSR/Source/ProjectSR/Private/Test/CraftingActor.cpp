// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/CraftingActor.h"
#include "Component/CraftingComponent.h"

ACraftingActor::ACraftingActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CraftingComponent = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));
}

void ACraftingActor::Interact_Implementation(AActor* InTarget)
{

}

void ACraftingActor::BeginPlay()
{
    Super::BeginPlay();
}

void ACraftingActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
