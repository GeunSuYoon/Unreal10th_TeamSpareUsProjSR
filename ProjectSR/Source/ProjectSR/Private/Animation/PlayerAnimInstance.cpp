// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PlayerAnimInstance.h"
#include "Player/PlayerCharacter.h"
#include "Component/InSpaceMovementComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance()
	: GroundSpeed(0.0f)
	, Velocity(FVector::ZeroVector)
	, bIsFalling(false)
	, bIsAccelerating(false)
	, bIsBoosting(false)
	, bIsZeroGravity(false)
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 최초 1회 캐릭터 및 이동 컴포넌트 캐싱
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		// 프로젝트의 Custom MovementComponent 가져오기 함수 호출
		MovementComponent = PlayerCharacter->GetInSpaceMovementComponent();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 캐릭터 참조가 없으면 재취득 시도 (에디터 내 시뮬레이션 지원)
	if (!PlayerCharacter)
	{
		PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
		if (PlayerCharacter)
		{
			MovementComponent = PlayerCharacter->GetInSpaceMovementComponent();
		}
	}

	if (!PlayerCharacter || !MovementComponent)
	{
		return;
	}

	// 1. Velocity 및 GroundSpeed 계산
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D(); // Z축을 제외한 평면 속도

	// 2. 가속 상태 (이동 키 입력 여부)
	bIsAccelerating = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0.0f;

	// 3. 공중 낙하 상태
	bIsFalling = MovementComponent->IsFalling();

	// 4. PlayerCharacter의 상태값 연동 (부스트 여부)
	bIsBoosting = PlayerCharacter->IsBoosting(); 

	// 5. 무중력 모드 (InSpaceMovementComponent의 GravityState 확인)
	bIsZeroGravity = (MovementComponent->GetGravityState() == EGravityState::ZeroGravityMode);
}
