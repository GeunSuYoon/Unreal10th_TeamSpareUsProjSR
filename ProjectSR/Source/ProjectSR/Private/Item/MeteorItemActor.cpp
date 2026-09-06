// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MeteorItemActor.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "ProjectSR.h"

AMeteorItemActor::AMeteorItemActor()
{
	this->SphereCollision_->SetCollisionObjectType(ECC_MeteorActor);
	this->SphereCollision_->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SphereCollision_->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SphereCollision_->SetCollisionResponseToChannel(ECC_SpaceShipActor, ECR_Overlap);
	this->SphereCollision_->SetGenerateOverlapEvents(true);
	this->SphereCollision_->OnComponentBeginOverlap.AddUniqueDynamic(
		this,
		&AMeteorItemActor::OnSphereBeginOverlap
	);
}

void AMeteorItemActor::InitMeteor(const FMeteor& InMeteor, const FVector& ShipCenter, float InDespawnDist)
{
	//this->ClosestApproachWorldPos__ = ShipCenter + InMeteor.ClosestApproachPos;
	this->MoveDir__ = InMeteor.MoveDir;
	this->DespawnDist__ = InDespawnDist;
	this->Damage__ = InMeteor.MeteorDamage;
	this->SphereCollision_->SetSphereRadius(InMeteor.MeteorSize);
}

void AMeteorItemActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//FVector	FromClosestPoint = GetActorLocation() - this->ClosestApproachWorldPos__;
	//float	PassedDistance = FVector::DotProduct(FromClosestPoint, this->MoveDir__);

	//if (PassedDistance >= this->DespawnDist__)
	//{
	//	FinishUsingPoolable();
	//}
}

void AMeteorItemActor::HandleImpact(ASpaceShipActor* InSpaceShipActor)
{
	if (this->bImpactResolved__)
	{
		return;
	}
	this->bImpactResolved__ = true;
	FVector ImpactLocation = GetActorLocation();

	// 중복 충돌 방지
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetRelativeVelocity(FVector::ZeroVector);

	if (ImpactVFX__)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ImpactVFX__,
			ImpactLocation,
			GetActorRotation(),
			FVector::OneVector,
			true,
			true
		);
	}
	if (ImpactSFX__)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSFX__, ImpactLocation);
	}
	if (ImpactCameraShake__)
	{
		if (APlayerController* PC =	UGameplayStatics::GetPlayerController(this, 0))
		{
			if (PC->PlayerCameraManager)
			{
				PC->PlayerCameraManager->StartCameraShake(ImpactCameraShake__);
			}
		}
	}
	// 우주선에 Damage__ 적용
	UGameplayStatics::ApplyDamage(
		InSpaceShipActor,
		this->Damage__,
		nullptr,
		this,
		nullptr
	);
	FinishUsingPoolable();
}

void AMeteorItemActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!IsValid(OtherActor))
	{
		return;
	}
	// 실제 우주선 또는 SafeArea Actor인지 검사
	if (ASpaceShipActor* SpaceShipActor = Cast<ASpaceShipActor>(OtherActor))
	{
		this->HandleImpact(SpaceShipActor);
	}
}

void AMeteorItemActor::OnSpawnFromPool_Implementation()
{
	Super::OnSpawnFromPool_Implementation();

	this->bImpactResolved__ = false;
}

void AMeteorItemActor::OnReturnToPool_Implementation()
{
	SetRelativeVelocity(FVector::ZeroVector);
	this->bImpactResolved__ = false;

	Super::OnReturnToPool_Implementation();
}

void AMeteorItemActor::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComponent, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (bImpactResolved__ ||
		!IsValid(OtherActor) ||
		!IsValid(OtherComponent))
	{
		return;
	}
	if (OtherComponent &&
		OtherComponent->GetCollisionObjectType() == ECC_SpaceShipActor)
	{
		if (ASpaceShipActor* SpaceShip =
			Cast<ASpaceShipActor>(OtherActor))
		{
			HandleImpact(SpaceShip);
		}
	}
}

