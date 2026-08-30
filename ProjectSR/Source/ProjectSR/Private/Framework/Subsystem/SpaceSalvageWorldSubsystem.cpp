// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "Framework/Subsystem/ItemActorFactorySubsystem.h"
#include "RootActor/SpaceRootActor.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Data/SpaceMap/SpaceMapItemSpawnRateDataAsset.h"
#include "Item/ItemActor.h"

bool USpaceSalvageWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	Super::ShouldCreateSubsystem(Outer);
	const UWorld*	World = Cast<UWorld>(Outer);

	UE_LOG(LogTemp, Warning, TEXT("SpaceSalvageWorldSubsystem 실행"));

	return (IsValid(World) && World->IsGameWorld());
}

void USpaceSalvageWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("SpaceSalvageSubsystem Created | World: %s | Type: %d | NetMode: %d"),
		*GetWorld()->GetName(),
		static_cast<int32>(GetWorld()->WorldType),
		static_cast<int32>(GetWorld()->GetNetMode())
	);
}

void USpaceSalvageWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	this->SpawnSpaceRoot__();
}

void USpaceSalvageWorldSubsystem::Deinitialize()
{

	Super::Deinitialize();
}

void USpaceSalvageWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void USpaceSalvageWorldSubsystem::SetSpaceMapData(USpaceMapItemSpawnRateDataAsset* InItemRateData)
{
	if (!InItemRateData)
	{
		return ;
	}
	this->ItemSpawnRateData__ = InItemRateData;
	this->ItemSpawnTimer__ = InItemRateData->ItemSpawnTime;
	this->ItemSpawnDist__ = InItemRateData->ItemSpawnDist;
	this->ItemMoveSpeed__ = InItemRateData->ItemMoveSpeed;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		this->ItemSpawnHandler__,
		this,
		&USpaceSalvageWorldSubsystem::SpawnItemActor__,
		1.0f,
		true,
		this->ItemSpawnTimer__
	);
	UE_LOG(LogTemp, Log, TEXT("MapData %s가 할당됐습니다."), *InItemRateData->MapName.ToString());
}

void USpaceSalvageWorldSubsystem::RegisterSpaceShipActor(ASpaceShipActor* InSpaceShip)
{
	if (!IsValid(InSpaceShip))
	{
		return;
	}
	this->SpaceShipActor__ = InSpaceShip;
	this->SpaceShipActor__.Get()->OnSpaceShipRotate.BindUFunction(this->SpaceRootActor__, TEXT("RotateSpaceRoot"));
}

void USpaceSalvageWorldSubsystem::RegisterMeteorAvoidance(UMeteorAvoidanceComponent* InAvoidanceComponent)
{
}

//void USpaceSalvageWorldSubsystem::RotateSpaceRoot(const FVector2D& InRotationInput)
//{
//	if (!IsValid(this->SpaceRootActor__))
//	{
//		return;
//	}
//	//this->SpaceRootActor__()
//}

void USpaceSalvageWorldSubsystem::RegisterVirtualMeteor(const FMeteor& InMeteor)
{
	this->ActiveVirtualMeteors__.Add(InMeteor);
	this->MeteorAvoidanceComponent__.Get()->MeteorDetect(InMeteor);
}

void USpaceSalvageWorldSubsystem::EndOfDay()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.ClearTimer(this->ItemSpawnHandler__);
}

void USpaceSalvageWorldSubsystem::SpaceShipRotateDetect(const FRotator& InRotate)
{
	if (this->SpaceRootActor__)
	{
		this->SpaceRootActor__->RotateSpaceRoot(InRotate);
	}
	if (this->MeteorAvoidanceComponent__)
	{
		//this->MeteorAvoidanceComponent__->
	}
}

void USpaceSalvageWorldSubsystem::SpawnSpaceRoot__()
{
	if (this->SpaceRootActor__)
	{
		return ;
	}
	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpaceRootActor*	SpaceRootActor = GetWorld()->SpawnActor<ASpaceRootActor>(
		ASpaceRootActor::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	this->SpaceRootActor__ = SpaceRootActor;
	UE_LOG(LogTemp, Log, TEXT("SpaceRootActor %s가 할당됐습니다."), *this->SpaceRootActor__.GetName());
}

void USpaceSalvageWorldSubsystem::SpawnItemActor__()
{
	UE_LOG(LogTemp, Log, TEXT("아이템 스폰 시작."));
	// 테스트용. 나중에 주석 지워야함
	//if (!this->SpaceShipActor__)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("SpaceShipActor가 nullptr입니다."));
	//	return ;
	//}
	UItemActorFactorySubsystem* ItemFactory = GetWorld()->GetSubsystem<UItemActorFactorySubsystem>();

	if (!ItemFactory)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemFactory가 nullptr입니다."));
		return ;
	}
	USceneComponent* ItemPivot = SpaceRootActor__->GetItemPivot();

	if (!IsValid(ItemPivot))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPivot이 nullptr입니다."));
		return ;
	}
	// 테스트용, 나중에 지우고 주석처리한 코드로 교체해야함
	FVector	SpaceShipForward = this->SpaceRootActor__->GetActorForwardVector();
	//FVector	SpaceShipForward = this->SpaceShipActor__->GetActorForwardVector();
	FVector	SpaceShipBackward = -1.0f * SpaceShipForward;

	SpaceShipForward.Y += FMath::FRandRange(-0.2, 0.2);
	SpaceShipForward.Z += FMath::FRandRange(-0.2, 0.2);
	FVector		SpawnPos = SpaceShipForward * this->ItemSpawnDist__;
	FTransform	WorldSpawnTransform(FRotator::ZeroRotator, SpawnPos);

	SpaceShipBackward.Y += FMath::FRandRange(-0.3, 0.3);
	SpaceShipBackward.Z += FMath::FRandRange(-0.3, 0.3);

	const float ItemSpeed =
		this->ItemMoveSpeed__ * FMath::FRandRange(0.8, 1.2);
	const FVector Velocity = SpaceShipBackward * ItemSpeed;

	TWeakObjectPtr<USceneComponent> WeakPivot(ItemPivot);
	UItemDataAsset*	TargetItemData = this->SelectSpawnItemData__();

	ItemFactory->SpawnItemActorAsync(
		TargetItemData,
		WorldSpawnTransform,
		FOnPickupSpawned::CreateWeakLambda(
			this,
			[WeakPivot, Velocity](AItemActor* ItemActor)
			{
				if (!IsValid(ItemActor))
				{
					return;
				}
				if (!WeakPivot.IsValid())
				{
					ItemActor->Destroy();
					return;
				}
				ItemActor->AttachToComponent(WeakPivot.Get(), FAttachmentTransformRules::KeepWorldTransform);				ItemActor->SetRelativeVelocity(Velocity);
				ItemActor->SetRelativeVelocity(Velocity);

				UE_LOG(LogTemp, Log, TEXT("아이템 %s 생성 위치: %s"), *ItemActor->GetName(), *ItemActor->GetActorLocation().ToString()	);
			})
	);
}

void USpaceSalvageWorldSubsystem::UpdateVirtualMeteors__(float CurrentWorldTime)
{
}

void USpaceSalvageWorldSubsystem::ResolveMeteor__(FMeteor& Meteor)
{
}

UItemDataAsset* USpaceSalvageWorldSubsystem::SelectSpawnItemData__()
{
	if (!this->ItemSpawnRateData__)
	{
		UE_LOG(LogTemp, Log, TEXT("ItemSpawnRateData가 nullptr입니다."));
		return (nullptr);
	}
	int32	TotalWeight = 0;

	for (auto& RateData : this->ItemSpawnRateData__->ItemSpawnRate)
	{
		TotalWeight += RateData.Value;
	}
	if (TotalWeight <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("TotalWeight가 0 이하입니다."));
		return (nullptr);
	}
	int32	RandomWeight = FMath::RandRange(0, TotalWeight);

	for (auto& RateData : this->ItemSpawnRateData__->ItemSpawnRate)
	{
		if (!IsValid(RateData.Key) || RateData.Value <= 0)
		{
			continue ;
		}
		RandomWeight -= RateData.Value;

		if (RandomWeight <= 0)
		{
			return (RateData.Key.Get());
		}
	}
	UE_LOG(LogTemp, Log, TEXT("아이템이 생성되지 않았습니다."));
	return (nullptr);
}
