// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InSpaceMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Player/PlayerCharacter.h"

UInSpaceMovementComponent::UInSpaceMovementComponent()
{
}

EGravityState UInSpaceMovementComponent::GetGravityState() const
{
	const bool bIsZeroGravity = (MovementMode == MOVE_Custom) && (CustomMovementMode == CMOVE_ZeroGravity);
	return bIsZeroGravity ? EGravityState::ZeroGravityMode : EGravityState::GravityMode;
}

// 무중력 상태 진입
void UInSpaceMovementComponent::EnterZeroGravity()
{
	// 이미 무중력 상태면 무시
	if (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_ZeroGravity)
	{
		return;
	}
	SetMovementMode(MOVE_Custom, CMOVE_ZeroGravity);
}

// 무중력 상태 탈출
void UInSpaceMovementComponent::ExitZeroGravity()
{
	if (MovementMode != MOVE_Custom || CustomMovementMode != CMOVE_ZeroGravity)
	{
		return;
	}
	// CMC가 바닥 감지 후 자동 전환
	SetMovementMode(MOVE_Falling);
}

float UInSpaceMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_ZeroGravity)
	{
		// 우주복에 따라 달라지는 StatComponent의 Effective 값과 연동 예정
		return ZeroGravityMaxSpeed;
	}
	return Super::GetMaxSpeed();
}

// 커스텀 모드로 라우팅
void UInSpaceMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case CMOVE_ZeroGravity:
		PhysZeroGravity(deltaTime, Iterations);
		break;
	default:
		Super::PhysCustom(deltaTime, Iterations);
		break;
	}
}

void UInSpaceMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	ACharacter* Owner = GetCharacterOwner();
	if (!Owner) return;

	const bool bEnteringZeroGravity = (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_ZeroGravity);
	const bool bExitingZeroGravity = (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_ZeroGravity);

	if (bEnteringZeroGravity)
	{
		GravityScale = 0.f;
		bOrientRotationToMovement = false;			// 무중력에선 이동 방향으로 자동 회전 끄고
		Owner->bUseControllerRotationYaw = true;	// 좌우상하 카메라를 따라 향하도록
		Owner->bUseControllerRotationPitch = true;	// 바꾸려면 true-false 스위치하면 됨
	}
	else if (bExitingZeroGravity)
	{
		GravityScale = DefaultGravityScale;
		bOrientRotationToMovement = true;
		Owner->bUseControllerRotationYaw = false;
		Owner->bUseControllerRotationPitch = false;
	}

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(Owner))
	{
		Player->HandleGravityStateChanged(bEnteringZeroGravity);
	}
}

// 무중력 상태시 물리 계산
void UInSpaceMovementComponent::PhysZeroGravity(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// 마찰 적용, 가속도 계산
	CalcVelocity(deltaTime, ZeroGravityFriction, false, ZeroGravityBrakingDeceleration);

	bJustTeleported = false;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	// 물체 충돌시 약간 튕겨나도록
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		
		// 반발계수
		const float Restitution = 0.3f;
		Velocity = FVector::VectorPlaneProject(Velocity, Hit.Normal) - (Hit.Normal * FVector::DotProduct(Velocity, Hit.Normal) * Restitution);
		// 남은 이동량만큼 미끄러지기
		SlideAlongSurface(Adjusted, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}

	if (!bJustTeleported)
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

