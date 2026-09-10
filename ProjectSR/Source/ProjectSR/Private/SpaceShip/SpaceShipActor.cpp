// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "SpaceShip/SpaceShipVisualActor.h"
#include "SpaceShip/LazerComponent.h"
#include "SpaceShip/MachineArmComponent.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "SpaceShip/DoorButtonActor.h"
#include "Component/InventoryComponent.h"
#include "Component/CraftingComponent.h"
#include "Component/InSpaceMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MainPanel/MainPanelActor.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"

#include "Components/SphereComponent.h"
#include "ProjectSR.h"
#include "Framework/SurvivalLoopActor.h"

bool ASpaceShipActor::IsDoorClosed() const
{
	return !bIsDoorOpen_ && !GetWorldTimerManager().IsTimerActive(DoorMoveTimerHandle_);
}

// Sets default values
ASpaceShipActor::ASpaceShipActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent*	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SetRootComponent(RootSceneComponent);
	this->SafeArea_ = CreateDefaultSubobject<USphereComponent>(TEXT("SafeArea"));
	this->SafeArea_->SetSphereRadius(this->SafeAreaRadius_);
	this->SafeArea_->SetupAttachment(GetRootComponent());
	this->SafeArea_->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->SafeArea_->SetCollisionObjectType(ECC_SpaceShipActor);
	this->SafeArea_->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SafeArea_->SetCollisionResponseToChannel(ECC_MeteorActor, ECR_Overlap);
	this->SafeArea_->SetGenerateOverlapEvents(true);

	this->SpaceShipVisualActor_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpaceShipVisual"));
	this->SpaceShipVisualActor_->SetupAttachment(GetRootComponent());

	this->MainPanel_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("MainPanel"));
	this->MainPanel_->SetupAttachment(GetRootComponent());

	this->DoorButton_ = CreateDefaultSubobject<UChildActorComponent>(TEXT("DoorButton"));
	this->DoorButton_->SetupAttachment(GetRootComponent());

	this->DoorMesh_ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	this->DoorMesh_->SetupAttachment(GetRootComponent());

	this->LazerComponent_ = CreateDefaultSubobject<ULazerComponent>(TEXT("LazerComponent"));
	this->UpgradeComponent_ = CreateDefaultSubobject<USpaceShipUpgradeComponent>(TEXT("UpgradeComponent"));
	this->MachineArmComponent_ = CreateDefaultSubobject<UMachineArmComponent>(TEXT("MainArmComponent"));
	this->WarehouseComponent_ = CreateDefaultSubobject<UInventoryComponent>(TEXT("WarehouseComponent"));
	this->MeteorAvoidanceComponent_ = CreateDefaultSubobject<UMeteorAvoidanceComponent>(TEXT("MeteorAvoidanceComponent"));
	this->CraftingComponent_ = CreateDefaultSubobject<UCraftingComponent>(TEXT("CraftingComponent"));
}

// Called when the game starts or when spawned
void ASpaceShipActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (this->MainPanel_)
	{
		this->MainPanelActor_ = Cast<AMainPanelActor>(this->MainPanel_->GetChildActor());
		if (this->MainPanelActor_ == nullptr)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[ASpaceShipActor::BeginPlay] MainPanel의 실제 타입이 AMainPanelActor가 아닙니다.")
			);
		}
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] MainPanel이 nullptr입니다.")
		);
	}
	if (this->DoorButton_)
	{
		this->DoorButtonActor_ = Cast<ADoorButtonActor>(this->DoorButton_->GetChildActor());
		if (this->DoorButtonActor_ == nullptr)
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[ASpaceShipActor::BeginPlay] DoorButton의 실제 타입이 ADoorButtonActor가 아닙니다.")
			);
		}
		this->DoorButtonActor_->OnDoorButtonClick.BindUFunction(this, TEXT("DetectDoorButtonClick_"));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] DoorButton이 nullptr입니다.")
		);
	}
	if (this->DoorMesh_)
	{
		DoorTargetRotate_ = this->DoorMesh_->GetRelativeRotation();
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] DoorMesh가 nullptr입니다.")
		);
	}
	if (this->MeteorAvoidanceComponent_)
	{
		this->MeteorAvoidanceComponent_->SetSpaceShipSafeArea(this->SafeAreaRadius_);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] MeteorAvoidanceComponent가 nullptr입니다.")
		);
	}
	// Initialize before publishing the ship to the world subsystem.
	if (bInitializeLevelZeroOnBeginPlay && UpgradeComponent_ && !UpgradeComponent_->InitializeLevelZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Level-zero initialization failed. Check the level-zero row, stats, recipes, and current level."), *GetName());
	}

	USpaceSalvageWorldSubsystem* SpaceSubsystem = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();

	if (!IsValid(SpaceSubsystem))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ASpaceShipActor::BeginPlay] SpaceSalvageWorldSubsystem을 찾지 못했습니다.")
		);
		return;
	}
	SpaceSubsystem->RegisterSpaceShipActor(this);
	SpaceSubsystem->SetSafeArea(this->SafeAreaRadius_);
}

// Called every frame
void ASpaceShipActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ASpaceShipActor::TakeDamage(
	float DamageAmount, 
	FDamageEvent const& DamageEvent, 
	AController* EventInstigator, 
	AActor* DamageCauser)
{
	float	Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	IDurabilityInterface::Execute_ConsumDurability(this, Damage);
	return (Damage);
}

UInventoryComponent* ASpaceShipActor::GetInventoryComponent_Implementation()
{
	return (this->WarehouseComponent_);
}

float ASpaceShipActor::RequestEnergy(float InEnergy)
{
	float	RetEnergy = FMath::Min(this->CurrentEnergy_, InEnergy);

	this->CurrentEnergy_ -= RetEnergy;
	return (RetEnergy);
}

void ASpaceShipActor::RepairDurability_Implementation(float InDurability)
{
	this->CurrentDurability_ = FMath::Min(this->CurrentDurability_ + InDurability, this->SpaceShipStat_.MaxDurability);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ASpaceShipActor::RepairDurability_Implementation] 들어온 내구도 수리 : [%.1f], 현재 내구도 : [%.1f]"),
		InDurability,
		this->CurrentDurability_
	);
	OnDurabilityChange.Broadcast(this->CurrentDurability_, this->SpaceShipStat_.MaxDurability);
	// 우주선 내구도가 0이 되면 터지는 로직 추가 필요
}

void ASpaceShipActor::ConsumDurability_Implementation(float InDurability)
{
	this->CurrentDurability_ = FMath::Max(this->CurrentDurability_ - InDurability, 0.0f);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ASpaceShipActor::ConsumDurability_Implementation] 들어온 내구도 데미지 : [%.1f], 현재 내구도 : [%.1f]"),
		InDurability,
		this->CurrentDurability_
	);
	OnDurabilityChange.Broadcast(this->CurrentDurability_, this->SpaceShipStat_.MaxDurability);
}

void ASpaceShipActor::GainEnergy(float InEnergy)
{
	this->CurrentEnergy_ = FMath::Min(this->CurrentEnergy_ + InEnergy, this->SpaceShipStat_.MaxEnergy);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ASpaceShipActor::GainEnergy] 들어온 추가 에너지 : [%.1f], 현재 에너지 : [%.1f]"),
		InEnergy,
		this->CurrentEnergy_
	);
	OnEnergyChange.Broadcast(this->CurrentEnergy_, this->SpaceShipStat_.MaxEnergy);
}

void ASpaceShipActor::UseEnergy(float InEnergy)
{
	this->CurrentEnergy_ = FMath::Max(this->CurrentEnergy_ - InEnergy, 0.0f);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ASpaceShipActor::UseEnergy] 들어온 소모 에너지 : [%.1f], 현재 에너지 : [%.1f]"),
		InEnergy,
		this->CurrentEnergy_
	);
	OnEnergyChange.Broadcast(this->CurrentEnergy_, this->SpaceShipStat_.MaxEnergy);
}

void ASpaceShipActor::SpaceShipMoveInput(const FVector2D& InInput)
{
	this->MeteorAvoidanceComponent_->SpaceShipMoveInput(InInput, this->SpaceShipStat_.MoveSpeed);
}

void ASpaceShipActor::SetSpaceShipData(USpaceShipDataAsset* InSpaceShipData)
{
	if (InSpaceShipData)
	{
		this->SpaceShipStat_.Level = InSpaceShipData->SpaceShipStat.Level;
		this->SpaceShipStat_.MaxDurability = InSpaceShipData->SpaceShipStat.MaxDurability;
		this->CurrentDurability_ = InSpaceShipData->SpaceShipStat.MaxDurability;
		this->SpaceShipStat_.MaxEnergy = InSpaceShipData->SpaceShipStat.MaxEnergy;
		this->CurrentEnergy_ = InSpaceShipData->SpaceShipStat.MaxEnergy;
		this->SpaceShipStat_.MoveSpeed = InSpaceShipData->SpaceShipStat.MoveSpeed;
		this->SpaceShipStat_.OperationalEnergy = InSpaceShipData->SpaceShipStat.OperationalEnergy;
	}
	else
	{
		this->SpaceShipStat_.Level = 0;
		this->SpaceShipStat_.MaxDurability = 0.0f;
		this->CurrentDurability_ = 0.0f;
		this->SpaceShipStat_.MaxEnergy = 0.0f;
		this->CurrentEnergy_ = 0.0f;
		this->SpaceShipStat_.MoveSpeed = 0.0f;
		this->SpaceShipStat_.OperationalEnergy = 0.0f;
	}
	OnSpaceShipLevelChange.ExecuteIfBound(this->SpaceShipStat_);
}

void ASpaceShipActor::ApplySpaceShipStat(const FSpaceShipStat& NewStat)
{
	SpaceShipStat_ = NewStat;
	Level_ = NewStat.Level;
	CurrentDurability_ = NewStat.MaxDurability;
	CurrentEnergy_ = NewStat.MaxEnergy;
	UpdateSpaceShipLevel();
}

void ASpaceShipActor::UpdateSpaceShipLevel()
{
	this->OnSpaceShipLevelChange.ExecuteIfBound(this->SpaceShipStat_);
	this->OnDurabilityChange.Broadcast(this->CurrentDurability_, this->SpaceShipStat_.MaxDurability);
	this->OnEnergyChange.Broadcast(this->CurrentEnergy_, this->SpaceShipStat_.MaxEnergy);
	this->LazerComponent_->UpdateLazerLevel();
	this->MachineArmComponent_->UpdateMachineArmLevel();
}

void ASpaceShipActor::DetectDoorButtonClick_()
{
	if (!this->DoorMesh_)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[ADoorButtonActor::Interact_Implementation] DoorMesh가 설정되지 않았습니다.")
		);
		return ;
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[ADoorButtonActor::Interact_Implementation] 문이 열리거나 닫힙니다.")
	);

	FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();

	this->DoorTargetRotate_.Roll = this->bIsDoorOpen_
			? this->DoorCloseAngle_
			: this->DoorOpenAngle_;
	this->bIsDoorOpen_ = !this->bIsDoorOpen_;
	GetWorldTimerManager().SetTimer(
		this->DoorMoveTimerHandle_,
		this,
		&ASpaceShipActor::UpdateDoorRotation_,
		this->DoorInteractTime_,
		true
	);
}

void	ASpaceShipActor::UpdateDoorRotation_()
{ 
	FRotator	DoorRotation = this->DoorMesh_->GetRelativeRotation();
	FRotator	NowRotate = FMath::RInterpConstantTo(DoorRotation, this->DoorTargetRotate_, this->DoorInteractTime_, this->DoorRotationSpeed_);

	//UE_LOG(
	//	LogTemp,
	//	Log,
	//	TEXT("[ADoorButtonActor::Interact_Implementation] 문이 움직입니다.")
	//);
	this->DoorMesh_->SetRelativeRotation(NowRotate);
	if (FMath::IsNearlyEqual(NowRotate.Roll, this->DoorTargetRotate_.Roll))
	{
		//UE_LOG(
		//	LogTemp,
		//	Log,
		//	TEXT("[ADoorButtonActor::Interact_Implementation] 문이 움직입니다.")
		//);
		GetWorldTimerManager().ClearTimer(this->DoorMoveTimerHandle_);

		// Apply the completed door state, including when its direction changes mid-animation.
		if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			if (UInSpaceMovementComponent* Movement = Player->GetInSpaceMovementComponent())
			{
				if (this->bIsDoorOpen_)
				{
					Movement->EnterZeroGravity();
				}
				else
				{
					auto* Salvage = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
					if (Salvage && Salvage->SurvivalLoop.IsValid() && !Salvage->SurvivalLoop->IsPlayerInside()) { Movement->EnterZeroGravity(); }
					else { Movement->ExitZeroGravity(); }
				}
			}
		}
	}
}

void	ASpaceShipActor::MeteorDetect(const USpaceMapDataAsset* InMapData)
{
	this->MeteorAvoidanceComponent_->MeteorDetect(InMapData);
}

//void ASpaceShipActor::SpaceShipRotateInput_(const FVector2D& InInput)
//{
//	float	DeltaTime = GetWorld()->GetDeltaSeconds();
//
//	FVector2D	InputToRotate = InInput.GetClampedToMaxSize(1.0f) * this->RotateSpeed_ * DeltaTime;
//
//	this->SpaceShipRotateState_ += FRotator(
//		-InputToRotate.Y,	// Pitch
//		-InputToRotate.X,	// Yaw
//		0.0f				// Roll
//	);
//	this->OnSpaceShipRotate.ExecuteIfBound(this->SpaceShipRotateState_);
//	//AddActorLocalRotation(DeltaRotation);
//}
