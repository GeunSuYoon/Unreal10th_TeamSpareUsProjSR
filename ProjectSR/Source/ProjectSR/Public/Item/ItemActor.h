// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h"
#include "Interface/InteractInterface.h"
#include "ItemActor.generated.h"

class UItemDataAsset;
class USphereComponent;

UCLASS()
class PROJECTSR_API AItemActor : public AActor, public IPoolableInterface, public IInteractInterface
{
    GENERATED_BODY()

public:
    AItemActor();

    virtual void FinishUsingPoolable() override;
    virtual void OnSpawnFromPool_Implementation() override;
    virtual void OnReturnToPool_Implementation() override;

    virtual void InitializeItemActor(const UItemDataAsset* InItemData);
    virtual void Interact_Implementation(AActor* InTarget) override;

protected:
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereCollision_ = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<const UItemDataAsset> ItemData_ = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

};
