// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UItemDataAsset;
class USphereComponent;

UCLASS()
class PROJECTSR_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

    virtual void InitializeItemActor(const UItemDataAsset* InItemData);

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



};
