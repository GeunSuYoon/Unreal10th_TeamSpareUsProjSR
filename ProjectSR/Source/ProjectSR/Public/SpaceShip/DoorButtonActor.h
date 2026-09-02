// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/InteractInterface.h"

#include "GameFramework/Actor.h"
#include "DoorButtonActor.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_DELEGATE(FOnDoorButtonClick);

UCLASS()
class PROJECTSR_API ADoorButtonActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorButtonActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void	Interact_Implementation(AActor* InTarget) override;

	FOnDoorButtonClick	OnDoorButtonClick;

protected:
	//void	UpdateDoorRotation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent>	InteractSphere_ = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float	InteractSphereRadius_ = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent>	ButtonMesh_ = nullptr;

};
