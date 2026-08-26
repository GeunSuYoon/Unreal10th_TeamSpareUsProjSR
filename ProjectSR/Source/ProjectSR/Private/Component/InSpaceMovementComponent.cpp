// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InSpaceMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	if (MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_ZeroGravity)
	{
		GravityScale = 0.f;
	}
	else if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_ZeroGravity)
	{
		GravityScale = DefaultGravityScale;		// 헤더에 저장해둔 기본값 복원
	}
}

// 무중력 상태시 물리 계산
void UInSpaceMovementComponent::PhysZeroGravity(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// 마찰만 적용해서 속도 계산 (중력 없음, Acceleration은 입력에서 누적된 값)
	CalcVelocity(deltaTime, ZeroGravityFriction, true, ZeroGravityBrakingDeceleration);

	bJustTeleported = false;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.Time < 1.f)
	{
		// 벽/장애물에 부딪히면 표면을 따라 미끄러지도록 처리
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}

	if (!bJustTeleported)
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

