// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "Interface/InventoryComponentInterface.h"
#include "StorageActor.generated.h"

class UInventoryComponent;

DECLARE_DYNAMIC_DELEGATE(FOnInteractStorage);

UCLASS()
class PROJECTSR_API AStorageActor : public AActor, public IInteractInterface, public IInventoryComponentInterface
{
    GENERATED_BODY()

public:
    AStorageActor();

    virtual void Interact_Implementation(AActor* InTarget) override;
    virtual UInventoryComponent* GetInventoryComponent_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    FOnInteractStorage OnInteractStorage;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UInventoryComponent> InventoryComponent;

private:
    bool bRegistered = false;
};
