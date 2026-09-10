// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "CraftingActor.generated.h"

class UCraftingComponent;

UCLASS()
class PROJECTSR_API ACraftingActor : public AActor, public IInteractInterface
{
    GENERATED_BODY()

public:
    ACraftingActor();

    virtual void Interact_Implementation(AActor* InTarget) override;

    inline UCraftingComponent* GetCraftingComponent() { return CraftingComponent; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    TObjectPtr<UCraftingComponent> CraftingComponent;

};
