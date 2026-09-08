// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatComponentInterface.h"
#include "Interface/InventoryComponentInterface.h"
#include "Interface/EquipComponentInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInSpaceMovementComponent;
class UStatComponent;
class UEquipComponent;
class UInventoryComponent;
class UInteractionComponent;

class UInputMappingContext;
class UInputAction;

DECLARE_DYNAMIC_DELEGATE(FOnToggleInventory);

UCLASS()
class PROJECTSR_API APlayerCharacter : public ACharacter,
	public IStatComponentInterface,
	public IInventoryComponentInterface,
	public IEquipComponentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 커스텀 무브먼트컴포넌트 편의 캐스팅
	UInSpaceMovementComponent* GetInSpaceMovementComponent() const;

	// StatComponent에서 부스트 상태 확인하는 함수
	bool IsBoosting() const { return bIsBoosting; }

	void HandleGravityStateChanged(bool bIsZeroGravity);

	// 중력 전환 테스트용 함수
	UFUNCTION(CallInEditor)
	void ToggleGravityMode();

protected:
	// Enhanced Input 액션 핸들러
	void Player_Move(const FInputActionValue& Value);
	void Player_Look(const FInputActionValue& Value);
	void Player_Jump(const FInputActionValue& Value);
	void Player_CrouchStart(const FInputActionValue& Value);
	void Player_CrouchStop(const FInputActionValue& Value);
	void Player_CrouchHold(const FInputActionValue& Value);
	void Player_BoostStart(const FInputActionValue& Value);
	void Player_BoostStop(const FInputActionValue& Value);
	void Player_Interact(const FInputActionValue& Value);
    void Player_Inventory(const FInputActionValue& Value);

	// 중력 상태에 따른 실제 이동 로직
	void Player_Move_Gravity(const FInputActionValue& Value);
	void Player_Move_ZeroGravity(const FInputActionValue& Value);

	void Player_Jump_Gravity(const FInputActionValue& Value);
	void Player_Jump_ZeroGravity(const FInputActionValue& Value);

	void Player_CrouchStart_Gravity(const FInputActionValue& Value);
	void Player_CrouchStop_Gravity(const FInputActionValue& Value);
	void Player_CrouchHold_ZeroGravity(const FInputActionValue& Value);


protected:
	// 컴포넌트 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UEquipComponent> EquipComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInteractionComponent> InteractionComponent;


	// Enhanced InputAction 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Crouch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Boost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interact;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_Inventory;

private:
	bool bIsBoosting = false;

public:
	// 종합 이동속도 갱신 함수
	void RefreshMovementSpeed();

public:
	// 각종 인터페이스 구현
	virtual UStatComponent* GetStatComponent_Implementation() override { return StatComponent; }

	virtual void IncreaseHP_Implementation(float InHP) override;
	virtual void DecreaseHP_Implementation(float InHP) override;
	virtual void ConsumOxigen_Implementation(float InOxigen) override;
	virtual void RecoverOxigen_Implementation(float InOxigen) override;

	virtual UInventoryComponent* GetInventoryComponent_Implementation() override { return InventoryComponent; }

	virtual UEquipComponent* GetEquipComponent_Implementation() override { return EquipComponent; }

public:
    FOnToggleInventory OnToggleInventory;
};
 