// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MeteorItemActor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "SpaceShip/SpaceShipActor.h"

void AMeteorItemActor::InitMeteor(const FMeteor& InMeteor, const FVector& ShipCenter, float InDespawnDist)
{
	//this->ClosestApproachWorldPos__ = ShipCenter + InMeteor.ClosestApproachPos;
	this->MoveDir__ = InMeteor.MoveDir;
	this->DespawnDist__ = InDespawnDist;
}

void AMeteorItemActor::LazerDamage(float InDamage)
{
	this->Damage__ -= InDamage;
	if (this->Damage__ <= 0.0f)
	{
		OnReturnToPool();
	}
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

void AMeteorItemActor::HandleImpact()
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
	if (OtherActor->IsA<ASpaceShipActor>())
	{
		this->HandleImpact();
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

