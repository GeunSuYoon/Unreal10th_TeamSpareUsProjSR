// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Component/InSpaceMovementComponent.h"
#include "Component/StatComponent.h"
#include "Component/EquipComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/InteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UInSpaceMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 하위 컴포넌트들 생성
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	EquipComponent = CreateDefaultSubobject<UEquipComponent>(TEXT("EquipComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Input Mapping Context 등록
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 부스트시 스탯 소모 로직
	//
	//
	//
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 앞뒤좌우 (WASD)
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_Move);
		// 시점 이동 (Mouse)
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_Look);
		// 점프 (Space)
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_Jump);
		// 웅크리기, 숨기 (좌Ctrl)
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_Crouch);
		// 부스트 (좌Shift)
		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Started, this, &APlayerCharacter::Player_BoostStart);
		EnhancedInputComponent->BindAction(IA_Boost, ETriggerEvent::Completed, this, &APlayerCharacter::Player_BoostStop);
		// 메인 상호작용 (F)
		EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &APlayerCharacter::Player_Interact);
	}
}

UInSpaceMovementComponent* APlayerCharacter::GetInSpaceMovementComponent() const
{
	return Cast<UInSpaceMovementComponent>(GetCharacterMovement());
}

void APlayerCharacter::Player_Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Player_Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Player_Jump(const FInputActionValue& Value)
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	// 중력 상태
	if (MoveComp->GetGravityState() == EGravityState::GravityMode)
	{
		Jump();
	}
	// 무중력 상태 : 위로 부유
	else
	{
		AddMovementInput(FVector::UpVector, 1.0f);
	}
}

void APlayerCharacter::Player_Crouch(const FInputActionValue& Value)
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	// 중력 상태
	if (MoveComp->GetGravityState() == EGravityState::GravityMode)
	{
		Crouch();
	}
	// 무중력 상태 : 아래로 하강
	else
	{
		AddMovementInput(FVector::UpVector, -1.0f);
	}
}

void APlayerCharacter::Player_BoostStart(const FInputActionValue& Value)
{
	bIsBoosting = true;
	// 무브먼트 속도 증가
}

void APlayerCharacter::Player_BoostStop(const FInputActionValue& Value)
{
	bIsBoosting = false;
	// 무브먼트 속도 복구
}

void APlayerCharacter::Player_Interact(const FInputActionValue& Value)
{
	InteractionComponent->PlayerInteract();
}

