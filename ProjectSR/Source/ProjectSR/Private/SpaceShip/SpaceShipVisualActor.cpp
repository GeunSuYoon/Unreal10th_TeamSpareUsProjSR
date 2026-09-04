// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/SpaceShipVisualActor.h"

// Sets default values
ASpaceShipVisualActor::ASpaceShipVisualActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASpaceShipVisualActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpaceShipVisualActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

