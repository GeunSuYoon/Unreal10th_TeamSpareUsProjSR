// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InSpaceMovementComponent.generated.h"

// 중력에 따른 이동모드 전환
UENUM(BlueprintType)
enum class EGravityState : uint8
{
	GravityMode,		// 중력 상태 - 우주선 내부
	ZeroGravityMode		// 무중력 상태 - 우주선 외부
};
/**
 * 
 */
UCLASS()
class PROJECTSR_API UInSpaceMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UInSpaceMovementComponent();

	static constexpr uint8 CMOVE_ZeroGravity = 0;

	UFUNCTION(BlueprintPure, Category = "Gravity")
	EGravityState GetGravityState() const;

	// 무중력 상태 진입
	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void EnterZeroGravity();

	// 무중력 상태 탈출
	UFUNCTION(BlueprintCallable, Category = "Gravity")
	void ExitZeroGravity();

	virtual float GetMaxSpeed() const override;


protected:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	void PhysZeroGravity(float deltaTime, int32 Iterations);
	

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ZeroGravity")
	float ZeroGravityFriction = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "ZeroGravity")
	float ZeroGravityBrakingDeceleration = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "ZeroGravity")
	float ZeroGravityMaxSpeed = 600.f;


private:
	UPROPERTY()
	float DefaultGravityScale = 1.f;
};
