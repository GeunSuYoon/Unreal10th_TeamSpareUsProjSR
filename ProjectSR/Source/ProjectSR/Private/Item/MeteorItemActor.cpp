// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MeteorItemActor.h"
#include "Camera/ImpactCameraShake.h"
#include "SpaceShip/SpaceShipActor.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectSR.h"
#include "Framework/SurvivalLoopActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"

AMeteorItemActor::AMeteorItemActor()
{
	ImpactCameraShake__ = UMeteorImpactCameraShake::StaticClass();

	MoveAudioComponent__ = CreateDefaultSubobject<UAudioComponent>(TEXT("MoveAudioComponent"));
	MoveAudioComponent__->SetupAttachment(GetRootComponent());
	MoveAudioComponent__->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<USoundBase> DefaultMoveSFX(
		TEXT("/Game/SFX/Asteroid/Loop/SFX_Asteroid_Loop01.SFX_Asteroid_Loop01")
	);
	if (DefaultMoveSFX.Succeeded())
	{
		MoveSFX__ = DefaultMoveSFX.Object;
	}

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
	this->ExitCenter__ = ShipCenter;
	this->bMeteorActive__ = true;
	this->DespawnDist__ = InDespawnDist;
	this->Damage__ = InMeteor.MeteorDamage;
	this->SphereCollision_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->SetActorScale3D(FVector::OneVector);
	if (UStaticMesh* MeteorVisual = MeteorVisualMesh__.LoadSynchronous())
	{
		this->Mesh->SetStaticMesh(MeteorVisual);
	}
	this->Mesh->SetVisibility(true, true);
	float MeteorRadius = InMeteor.MeteorSize * 0.5f;

	this->SphereCollision_->SetSphereRadius(MeteorRadius, true);
	if (this->Mesh && this->Mesh->GetStaticMesh())
	{
		float	MeshBaseRadius = this->Mesh->GetStaticMesh()->GetBounds().SphereRadius;

		if (MeshBaseRadius > UE_SMALL_NUMBER)
		{
			float MeshScale = MeteorRadius / MeshBaseRadius;

			this->Mesh->SetRelativeScale3D(FVector(MeshScale));
		}
	}
	this->SphereCollision_->SetCollisionObjectType(ECC_MeteorActor);
	this->SphereCollision_->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SphereCollision_->SetCollisionResponseToChannel(ECC_SpaceShipActor, ECR_Overlap);
	this->SphereCollision_->SetGenerateOverlapEvents(true);
	this->SphereCollision_->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SetActorEnableCollision(true);
	this->SphereCollision_->UpdateOverlaps();
	StartMoveSFX();
}

void AMeteorItemActor::StartMoveSFX()
{
	StopMoveSFX();
	MoveAudioComponent__->SetSound(MoveSFX__);
	if (MoveSFX__)
	{
		MoveAudioComponent__->Play();
	}
}

void AMeteorItemActor::StopMoveSFX()
{
	if (MoveAudioComponent__->IsPlaying())
	{
		MoveAudioComponent__->Stop();
	}
}

void AMeteorItemActor::LazerDamage(float InDamage)
{
	this->Damage__ = FMath::Max(0.0f, this->Damage__ - FMath::Max(0.0f, InDamage));
	if (this->Damage__ <= 0.0f)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[AMeteorItemActor::LazerDamage] 운석이 파괴됐습니다.")
		);
		FinishUsingPoolable();
	}
}

void AMeteorItemActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bMeteorActive__ && FVector::DotProduct(GetActorLocation() - ExitCenter__, MoveDir__) >= DespawnDist__) { FinishUsingPoolable(); }

	//FVector	FromClosestPoint = GetActorLocation() - this->ClosestApproachWorldPos__;
	//float	PassedDistance = FVector::DotProduct(FromClosestPoint, this->MoveDir__);

	//if (PassedDistance >= this->DespawnDist__)
	//{
	//	FinishUsingPoolable();
	//}
}

void AMeteorItemActor::HandleImpact(ASpaceShipActor* InSpaceShipActor)
{
	if (this->bImpactResolved__ || !bMeteorActive__)
	{
		return;
	}
	this->bImpactResolved__ = true;
	StopMoveSFX();
	FVector ImpactLocation = GetActorLocation();

	// 중복 충돌 방지
	this->SetActorEnableCollision(false);
	this->SetActorTickEnabled(false);
	this->SetRelativeVelocity(FVector::ZeroVector);

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
	TSubclassOf<UCameraShakeBase> CameraShakeClass = ImpactCameraShake__;
	if (!CameraShakeClass)
	{
		CameraShakeClass = UMeteorImpactCameraShake::StaticClass();
	}
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(CameraShakeClass);
		}
	}
	// Actual impact, not the warning timeout. Capture damage before game-over cleanup returns this actor.
	const float ImpactDamage = Damage__;
	bool bDamageHandledBySurvivalLoop = false;
	if (auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>())
	{
		if (Salvage->GetSurvivalLoop())
		{
			Salvage->GetSurvivalLoop()->NotifyMeteorImpact(InSpaceShipActor, ImpactDamage);
			bDamageHandledBySurvivalLoop = true;
		}
	}
	if (!bDamageHandledBySurvivalLoop)
	{
		UGameplayStatics::ApplyDamage(InSpaceShipActor, ImpactDamage, nullptr, this, nullptr);
	}
	if (bMeteorActive__) { FinishUsingPoolable(); }
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
	bMeteorActive__ = false;
	StopMoveSFX();
	SetActorEnableCollision(false);
}

void AMeteorItemActor::OnReturnToPool_Implementation()
{
	SetRelativeVelocity(FVector::ZeroVector);
	//this->bImpactResolved__ = false;
	bMeteorActive__ = false;
	StopMoveSFX();

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

