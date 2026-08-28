// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "ItemActor.generated.h"

class UItemDataAsset;
class USphereComponent;

UCLASS()
class PROJECTSR_API AItemActor : public AActor, public IInteractInterface
{
    GENERATED_BODY()

public:
    AItemActor();

    virtual void InitializeItemActor(const UItemDataAsset* InItemData);
    virtual void Interact_Implementation(AActor* InTarget) override;

	void	SetRelativeVelocity(const FVector& InVelocity);

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

private:
	FVector RelativeVelocity__;

};
