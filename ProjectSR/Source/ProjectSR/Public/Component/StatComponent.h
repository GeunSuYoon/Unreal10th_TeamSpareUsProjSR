// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"


// 스탯 변경 시 UI 업데이트용 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChangedSignature, float, CurrentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeathSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- 스탯 조절 함수 ---
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ModifyHealth(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ModifyHunger(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ModifyOxygen(float Amount);

	// 중력 상태 변화에 따라 캐릭터가 호출
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void SetOxygenConsuming(bool bShouldConsume) { bIsOxygenConsume = bShouldConsume; }

	// 장비 장착/해제시 EquipComponent가 호출
	// 장비의 스탯 보너스 추가 여부에 따라 파라미터를 구조체로 만들수도 --- 추후 논의
	UFUNCTION()
	void RecalculateMaxStats(float OxygenBonus, float HealthBonus);

	// --- Getter --- 스탯
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetHealth() const { return CurrentHealth; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetHunger() const { return CurrentHunger; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxHunger() const { return MaxHunger; }

	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetOxygen() const { return CurrentOxygen; }
	UFUNCTION(BlueprintPure, Category = "Stat")
	float GetMaxOxygen() const { return MaxOxygen; }

	// --- Getter --- 이동속도
	UFUNCTION(BlueprintPure, Category = "Stat|Movement") float GetMoveSpeed() const { return MoveSpeed; }
	UFUNCTION(BlueprintPure, Category = "Stat|Movement") float GetZeroGravityMoveSpeed() const { return ZeroGravityMoveSpeed; }
	UFUNCTION(BlueprintPure, Category = "Stat|Movement") float GetBoostSpeed() const { return BoostSpeed; }
	UFUNCTION(BlueprintPure, Category = "Stat|Movement") float GetCrouchSpeed() const { return CrouchSpeed; }

public:
	// UI 연동을 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedSignature OnHungerChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnStatChangedSignature OnOxygenChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stat|Events")
	FOnPlayerDeathSignature OnPlayerDeath;

protected:
	// --- Base 최대 스탯값 --- 장비 영향 X
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseMaxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseMaxOxygen = 150.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float MaxHunger = 100.0f;	// 장비 영향 없다고 가정
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseGravityMoveSpeed = 600.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseZeroGravityMoveSpeed = 400.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseBoostSpeed = 900.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseZeroGravityBoostSpeed = 700.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stat|Base") float BaseCrouchSpeed = 300.0f;


	// --- Effective 최대 스탯값 --- 장비 보정 반영된 실 최대값, 런타임 계산
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float MaxOxygen;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float MoveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float ZeroGravityMoveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float BoostSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float ZeroGravityBoostSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Effective") float CrouchSpeed;
	
	// --- 초당 스탯 소모율 ---
	// 기본 허기 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float BaseHungerDrainRate = 0.5f;
	// 부스트 시 추가 허기 감소 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float BoostHungerDrainMultiplier = 2.0f;
	// 무중력 상태시 산소 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float OxygenDrainRate = 2.0f;
	// 허기 고갈시 체력 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float StarvationDamageRate = 5.0f;
	// 산소 고갈시 체력 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float NoOxygenDamageRate = 10.0f;
	// 우주선 복귀시 산소 회복 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|DrainRate") float OxygenRecoverRate = 20.0f;

private:
	// 현재 스탯 값
	float CurrentHealth;
	float CurrentHunger;
	float CurrentOxygen;

	// 사망 여부 플래그
	bool bIsDead = false;
	// 중력, 무중력 여부에 따른 산소 소모 여부
	bool bIsOxygenConsume = false;

	// 매 프레임 캐스팅 방지용 캐시
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> OwnerCharacter;
};
