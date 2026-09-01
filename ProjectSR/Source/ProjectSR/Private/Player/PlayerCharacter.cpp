// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/InSpaceMovementComponent.h"
#include "Component/StatComponent.h"
#include "Component/EquipComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/InteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/Item/EquipmentDataAsset.h"

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

	// 웅크리기 기능 활성화
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// 카메라 세팅 - 캐릭터는 컨트롤러 회전을 그대로 따라가지 않고, 이동 방향으로만 자연스럽게 회전하도록 설정(변경 가능)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true; // 붐이 마우스 입력을 따라 회전

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 붐 회전만 따라감, 별도 회전 안 함
	
	// 카메라 이동 보정
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
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

	// 초기화된 StatComponent 기반으로 이동속도 적용
	if (StatComponent)
	{
		if (UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent())
		{
			MoveComp->MaxWalkSpeedCrouched = StatComponent->GetCrouchSpeed();
		}

		RefreshMovementSpeed();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter::BeginPlay - StatComponent is NULL!"));
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		// 점프, 상승 (Space)
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_Jump);
		// 웅크리기, 하강 (좌Ctrl)
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &APlayerCharacter::Player_CrouchStart);
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &APlayerCharacter::Player_CrouchStop);
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &APlayerCharacter::Player_CrouchHold);
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

void APlayerCharacter::HandleGravityStateChanged(bool bIsZeroGravity)
{
	if (!StatComponent) return;

	StatComponent->SetOxygenConsuming(bIsZeroGravity);

	RefreshMovementSpeed();
}

// 테스트용 함수
void APlayerCharacter::ToggleGravityMode()
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	// 현재 중력 상태 확인 후 토글
	if (MoveComp->GetGravityState() == EGravityState::GravityMode)
	{
		MoveComp->EnterZeroGravity();
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Switched to ZERO GRAVITY Mode"));
	}
	else
	{
		MoveComp->ExitZeroGravity();
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Switched to GRAVITY Mode"));
	}
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

void APlayerCharacter::Player_CrouchStart(const FInputActionValue& Value)
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	if (MoveComp->GetGravityState() == EGravityState::GravityMode)
	{
		Crouch();
	}
}

void APlayerCharacter::Player_CrouchStop(const FInputActionValue& Value)
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	if (MoveComp->GetGravityState() == EGravityState::GravityMode)
	{
		UnCrouch();
	}
}

// 무중력 상태시 하강하는 함수
void APlayerCharacter::Player_CrouchHold(const FInputActionValue& Value)
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp) return;

	if (MoveComp->GetGravityState() == EGravityState::ZeroGravityMode)
	{
		AddMovementInput(FVector::DownVector, 1.0f);
	}
}

void APlayerCharacter::Player_BoostStart(const FInputActionValue& Value)
{
	bIsBoosting = true;
	RefreshMovementSpeed();
}

void APlayerCharacter::Player_BoostStop(const FInputActionValue& Value)
{
	bIsBoosting = false;
	RefreshMovementSpeed();
}

void APlayerCharacter::Player_Interact(const FInputActionValue& Value)
{
	InteractionComponent->PlayerInteract();
}

void APlayerCharacter::RefreshMovementSpeed()
{
	UInSpaceMovementComponent* MoveComp = GetInSpaceMovementComponent();
	if (!MoveComp || !StatComponent) return;

	if (bIsBoosting)
	{
		MoveComp->MaxWalkSpeed = StatComponent->GetBoostSpeed();
		return;
	}

	const bool bIsZeroGravity = MoveComp->GetGravityState() == EGravityState::ZeroGravityMode;
	MoveComp->MaxWalkSpeed = bIsZeroGravity ? StatComponent->GetZeroGravityMoveSpeed() : StatComponent->GetMoveSpeed();
}

void APlayerCharacter::IncreaseHP_Implementation(float InHP)
{
	if (StatComponent)
	{
		StatComponent->ModifyHealth(InHP);
	}
}

void APlayerCharacter::DecreaseHP_Implementation(float InHP)
{
	if (StatComponent)
	{
		StatComponent->ModifyHealth(-InHP);
	}
}

void APlayerCharacter::ConsumOxigen_Implementation(float InOxigen)
{
	if (StatComponent)
	{
		StatComponent->ModifyOxygen(-InOxigen);
	}
}

void APlayerCharacter::RecoverOxigen_Implementation(float InOxigen)
{
	if (StatComponent)
	{
		StatComponent->ModifyOxygen(InOxigen);
	}
}

