// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Component/InSpaceMovementComponent.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */ 
UCLASS()
class PROJECTSR_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// BeginPlay 역할
	virtual void NativeInitializeAnimation() override;
	// Tick 역할
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 소유 캐릭터 참조 (캐스팅 비용 절감)
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	class APlayerCharacter* PlayerCharacter;

	// 이동 컴포넌트 참조
	UPROPERTY(BlueprintReadOnly, Category = "Reference")
	class UInSpaceMovementComponent* MovementComponent;

	// --- AnimGraph 및 StateMachine 연동용 변수들 ---

	// 평면 속도 (BlendSpace 입력용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	// 3D 속도 벡터 (무중력 이동에서 활용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	// 공중에 떠 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	// 이동중 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	// 부스트 상태 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsBoosting;

	// 무중력 상태 여부 (StateMachine 전환용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsZeroGravity;
};
