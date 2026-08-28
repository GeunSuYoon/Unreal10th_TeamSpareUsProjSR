// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Meteor.h"
#include "MeteorAvoidanceComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnMeteorDetect);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTSR_API UMeteorAvoidanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeteorAvoidanceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnMeteorDetect	OnMeteorDetect;

	void	MeteorDetect(const FMeteor& InMeteor);
	void	SpaceShipRotateChange(const FRotator& InSpaceShipRotate);
	void	EvaluateMeteorAvoidance();

protected:
	TArray<FMeteor>	TargetMeteorList_;

	FRotator	SpaceShipRotate_;

private:
	//void	
};