// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"
#include "Framework/Subsystem/ItemActorFactorySubsystem.h"
#include "Framework/SurvivalLoopActor.h"
#include "RootActor/SpaceRootActor.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Data/SpaceMap/SpaceMapDataAsset.h"
#include "Interface/PoolableInterface.h"
#include "Item/ItemActor.h"
#include "Item/MeteorItemActor.h"
#include "Utility/UtilFunction.h"
#include "SpaceShip/LazerComponent.h"
#include "Player/PlayerCharacter.h"
#include "TimerManager.h"

bool USpaceSalvageWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	Super::ShouldCreateSubsystem(Outer);
	const UWorld*	World = Cast<UWorld>(Outer);

	UE_LOG(LogTemp, 
		Log,
		TEXT("[USpaceSalvageWorldSubsystem::ShouldCreateSubsystem] 실행")
	);

	return (IsValid(World) && World->IsGameWorld());
}

void USpaceSalvageWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	this->SpawnSpaceRoot__();
}

void USpaceSalvageWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

}

void USpaceSalvageWorldSubsystem::Deinitialize()
{
	++SpawnGeneration__;
	bStartCheckScheduled__ = false;
	SurvivalLoop__ = nullptr;
	PlayerCharacter__ = nullptr;
	SpaceShipActor__ = nullptr;
	Super::Deinitialize();
}

void USpaceSalvageWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->MeteorSpawnTime__ <= 0.0f)
	{
		return ;
	}
	//if (this->LevelTime__ == 0.0f)
	//{
	//	this->MeteorDetect();
	//}
	//this->LevelTime__ += DeltaTime;
	//if (this->LevelTime__ >= this->MeteorSpawnTime__)
	//{
	//	this->MeteorSpawnTime__ *= 2.0f;
	//	this->MeteorDetect();
	//}
}

void USpaceSalvageWorldSubsystem::SetSafeArea(float InArea)
{
	this->SafeArea__ = InArea;
	this->SafeAreaSquared__ = FMath::Square(InArea);
}

void USpaceSalvageWorldSubsystem::SetSpaceMapData(USpaceMapDataAsset* InSpaceMapData)
{
	if (SurvivalLoop__)
	{
		UE_LOG(LogTemp, Warning, TEXT("The survival loop owns map changes. Remove the separate SetSpaceMapData call."));
		return;
	}
	StartDay(InSpaceMapData);
}

void USpaceSalvageWorldSubsystem::StartDay(USpaceMapDataAsset* InSpaceMapData)
{
	if (!InSpaceMapData)
	{
		UE_LOG(LogTemp,
			Warning,
			TEXT("[USpaceSalvageWorldSubsystem::SetSpaceMapData] MapData가 nullptr입니다."));
		return ;
	}
	EndOfDay();
	++SpawnGeneration__;
	for (const auto& Item : SpawnedItem__)
	{
		if (Item.IsValid() && !Item->IsHidden()) { Item->FinishUsingPoolable(); }
	}
	SpawnedItem__.Reset();
	bSpawningEnabled__ = true;
	this->SpaceMapData__ = InSpaceMapData;
	this->ItemSpawnTimer__ = InSpaceMapData->ItemSpawnTime;
	this->ItemSpawnDist__ = InSpaceMapData->ItemSpawnDist;
	this->ItemDespawnDistSquared__ = FMath::Square(InSpaceMapData->ItemSpawnDist * 1.5);
	this->ItemMoveSpeed__ = InSpaceMapData->ItemMoveSpeed;
	this->MeteorSpawnDelayTime__ = InSpaceMapData->MeteorSpawnDelayTime;
	this->MeteorSpawnTime__ = InSpaceMapData->MeteorSpawnTime;
	if (this->MeteorSpawnTime__ > 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();

		TimerManager.SetTimer(
			this->MeteorSpawnHandler__,
			this,
			&USpaceSalvageWorldSubsystem::MeteorDetect,
			this->MeteorSpawnTime__,
			true,
			this->MeteorSpawnDelayTime__
		);
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[USpaceSalvageWorldSubsystem::SetSpaceMapData] 운석 타이머가 설정됐습니다.")
		);
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[USpaceSalvageWorldSubsystem::SetSpaceMapData] MapData %s가 할당됐습니다."),
		*this->SpaceMapData__->MapName.ToString()
	);
	this->OnSpaceMapUpdate.ExecuteIfBound(this->ItemSpawnDist__);
	this->TryStartItemSpawn__();
}

void USpaceSalvageWorldSubsystem::RegisterSpaceShipActor(ASpaceShipActor* InSpaceShip)
{
	if (!IsValid(InSpaceShip))
	{
		return;
	}
	InSpaceShip->OnSpaceShipRotate.BindUFunction(this, TEXT("SpaceShipRotateDetect"));
	InSpaceShip->GetMeteorAvoidance()->OnMeteorCollision.AddUniqueDynamic(this, &USpaceSalvageWorldSubsystem::SpawnMeteor__);
	this->SpaceShipActor__ = InSpaceShip;
	this->TryStartItemSpawn__();
	this->CheckStartDay__();
}

void USpaceSalvageWorldSubsystem::RegisterSurvivalLoopActor(ASurvivalLoopActor* InSurvivalLoop)
{
	if (!IsValid(InSurvivalLoop)) return;
	if (IsValid(SurvivalLoop__) && SurvivalLoop__ != InSurvivalLoop)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple SurvivalLoop actors tried to register. Keep only one in the level."));
		return;
	}
	this->SurvivalLoop__ = InSurvivalLoop;
	this->CheckStartDay__();
}

void USpaceSalvageWorldSubsystem::UnregisterSurvivalLoopActor(ASurvivalLoopActor* InSurvivalLoop)
{
	if (SurvivalLoop__ != InSurvivalLoop) return;
	bStartCheckScheduled__ = false;
	SurvivalLoop__ = nullptr;
}

void USpaceSalvageWorldSubsystem::RegisterPlayer(APlayerCharacter* InPlayer)
{
	if (!IsValid(InPlayer)) return;
	this->PlayerCharacter__ = InPlayer;
	this->CheckStartDay__();
}

void USpaceSalvageWorldSubsystem::MeteorDetect()
{
	if (bSpawningEnabled__ && IsValid(SpaceMapData__) && IsValid(SpaceShipActor__) && !HasPendingMeteor() && FMath::FRand() < FMath::Clamp(SpaceMapData__->MeteorSpawnRate, 0.0f, 1.0f))
	{
		// 나중에 주석 해제해야함
		this->SpaceShipActor__->MeteorDetect(this->SpaceMapData__);
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[USpaceSalvageWorldSubsystem::MeteorDetect] 운석이 관측됐습니다.")
		);
	}
}

void USpaceSalvageWorldSubsystem::EndOfDay()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.ClearTimer(this->ItemSpawnHandler__);
	TimerManager.ClearTimer(this->MeteorSpawnHandler__);
	bSpawningEnabled__ = false;
}

void USpaceSalvageWorldSubsystem::ClearDayActors()
{
	EndOfDay();
	GetWorld()->GetTimerManager().ClearTimer(ItemDespawnHandler__);
	++SpawnGeneration__; // Invalidates late asynchronous item callbacks from the previous day.
	bMeteorLoading__ = false;

	for (const TWeakObjectPtr<AItemActor>& Item : SpawnedItem__)
	{
		if (Item.IsValid() && !Item->IsHidden()) Item->FinishUsingPoolable();
	}
	SpawnedItem__.Reset();

	if (ActiveMeteor__.IsValid() && ActiveMeteor__->IsMeteorActive())
	{
		ActiveMeteor__->FinishUsingPoolable();
	}
	ActiveMeteor__.Reset();

	if (IsValid(SpaceShipActor__) && SpaceShipActor__->GetMeteorAvoidance())
	{
		SpaceShipActor__->GetMeteorAvoidance()->ClearMeteor();
	}
}

void USpaceSalvageWorldSubsystem::SpaceShipRotateDetect(const FRotator& InRotate)
{
	if (this->SpaceRootActor__)
	{
		this->SpaceRootActor__->RotateSpaceRoot(InRotate);
	}
}

void USpaceSalvageWorldSubsystem::TryStartItemSpawn__()
{
	if (!bSpawningEnabled__ || !IsValid(this->SpaceMapData__)
		|| !IsValid(this->SpaceShipActor__)
		|| !IsValid(this->SpaceRootActor__))
	{
		return;
	}
	this->SpawnItemLevelStart__(this->SpaceMapData__->ItemSpawnInitCount);
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		this->ItemSpawnHandler__,
		this,
		&USpaceSalvageWorldSubsystem::SpawnItemActor__,
		this->ItemSpawnTimer__,
		true
	);
	TimerManager.SetTimer(
		this->ItemDespawnHandler__,
		this,
		&USpaceSalvageWorldSubsystem::DespawnItemActor__,
		this->ItemDespawnTimer__,
		true
	);
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
	UE_LOG(
		LogTemp, 
		Log, 
		TEXT("[USpaceSalvageWorldSubsystem::SpawnSpaceRoot__] SpaceRootActor %s가 할당됐습니다."),
		*this->SpaceRootActor__.GetName());
	this->TryStartItemSpawn__();
}

void USpaceSalvageWorldSubsystem::SpawnItemLevelStart__(int32 InitItemCount)
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[USpaceSalvageWorldSubsystem::SpawnItemLevelStart__] 아이템 스폰 시작.")
	);
	for (int32 SpawnCount = 0; SpawnCount < InitItemCount; SpawnCount++)
	{
		FVector	ItemSpawnPos;
		int32	TryCount = 0;
		do
		{
			TryCount++;
			ItemSpawnPos = FVector(
				FMath::FRandRange(-1.0f * this->ItemSpawnDist__, 1.0f * this->ItemSpawnDist__), 
				FMath::FRandRange(-1.0f * this->ItemSpawnDist__, 1.0f * this->ItemSpawnDist__), 
				FMath::FRandRange(-1.0f * this->ItemSpawnDist__, 1.0f * this->ItemSpawnDist__)
				);
		} while	(
			FVector::DistSquared(ItemSpawnPos, this->SpaceShipActor__->GetActorLocation()) < this->SafeAreaSquared__
			&& TryCount < this->ItemSpawnMaxRetryCount__);
		if (TryCount == this->ItemSpawnMaxRetryCount__)
		{
			continue ;
		}
		this->SpawnItemActor__(ItemSpawnPos);
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[USpaceSalvageWorldSubsystem::SpawnItemLevelStart__] 아이템 스폰 종료.")
	);
}

void USpaceSalvageWorldSubsystem::SpawnItemActor__()
{
	if (!bSpawningEnabled__ || !IsValid(this->SpaceMapData__)
		|| !IsValid(this->SpaceShipActor__)
		|| !IsValid(this->SpaceRootActor__))
	{
		return;
	}
	UE_LOG(
		LogTemp, 
		Log, 
		TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] 아이템 스폰 시작.")
	);
	UItemActorFactorySubsystem* ItemFactory = GetWorld()->GetSubsystem<UItemActorFactorySubsystem>();

	if (!ItemFactory)
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemFactory가 nullptr입니다.")
		);
		return ;
	}
	USceneComponent* ItemPivot = SpaceRootActor__->GetItemPivot();

	if (!IsValid(ItemPivot))
	{
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemPivot이 nullptr입니다."));
		return ;
	}
	FVector	ItemSpawnDir(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f));

	ItemSpawnDir.Normalize();
	FVector		ItemMoveDir = ItemSpawnDir * -1.0f;
	FVector		SpawnPos = this->SpaceShipActor__->GetActorLocation() + ItemSpawnDir * this->ItemSpawnDist__;
	int32		TryCount = 0;

	while (TryCount < this->ItemSpawnMaxRetryCount__)
	{
		++TryCount;

		FVector	TempDir = ItemMoveDir;
		TempDir.X += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Y += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Z += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Normalize();
		float	DistSquared = FUtilFunction::GetPointToLineDistanceSquared(
			this->SpaceShipActor__->GetActorLocation(),
			SpawnPos,
			TempDir
		);
		if (DistSquared > this->SafeAreaSquared__)
		{
			ItemMoveDir = TempDir;
			break;
		}

		if (TryCount == this->ItemSpawnMaxRetryCount__)
		{
			// 안전한 방향을 찾지 못하면 이번 스폰은 건너뜀
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] 안전한 아이템 속도를 찾지 못했습니다."));
			return;
		}
	}
	float							ItemSpeed =	this->ItemMoveSpeed__ * FMath::FRandRange(0.8, 1.2);
	FVector							Velocity = ItemMoveDir * ItemSpeed;
	TWeakObjectPtr<USceneComponent> WeakPivot(ItemPivot);
	UItemDataAsset*					TargetItemData = this->SelectSpawnItemData__();
	FTransform						WorldSpawnTransform(FRotator::ZeroRotator, SpawnPos);

	ItemFactory->SpawnItemActorAsync(
		TargetItemData,
		WorldSpawnTransform,
		FOnPickupSpawned::CreateWeakLambda(
			this,
			[this, WeakPivot, Velocity, Generation = SpawnGeneration__](AItemActor* ItemActor)
			{
				if (!IsValid(ItemActor))
				{
					UE_LOG(
						LogTemp,
						Error,
						TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemActor가 스폰되지 않았습니다.")
					);
					return;
				}
				if (!bSpawningEnabled__ || Generation != SpawnGeneration__ || !WeakPivot.IsValid())
				{
					UE_LOG(
						LogTemp,
						Error,
						TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] WeakPivot이 Valid하지 않습니다.")
					);
					ItemActor->FinishUsingPoolable();
					return;
				}
				ItemActor->AttachToComponent(
					WeakPivot.Get(), 
					FAttachmentTransformRules::KeepWorldTransform
				);
				ItemActor->SetRelativeVelocity(Velocity);
				this->SpawnedItem__.AddUnique(ItemActor);
				UE_LOG(
					LogTemp,
					Log,
					TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] 아이템 %s 생성 위치: %s"),
					*ItemActor->GetName(),
					*ItemActor->GetActorLocation().ToString()
				);
			}), [WeakThis = TWeakObjectPtr<USpaceSalvageWorldSubsystem>(this), Generation = SpawnGeneration__]()
		{
			return WeakThis.IsValid() && WeakThis->bSpawningEnabled__ && WeakThis->SpawnGeneration__ == Generation;
		}
	);
}

void	USpaceSalvageWorldSubsystem::SpawnItemActor__(FVector InLocation)
{
	if (!bSpawningEnabled__ || !this->SpaceShipActor__)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpaceShipActor가 nullptr입니다."));
		return ;
	}
	UItemActorFactorySubsystem* ItemFactory = GetWorld()->GetSubsystem<UItemActorFactorySubsystem>();

	if (!ItemFactory)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemFactory가 nullptr입니다.")
		);
		return;
	}
	USceneComponent* ItemPivot = SpaceRootActor__->GetItemPivot();

	if (!IsValid(ItemPivot))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemPivot이 nullptr입니다."));
		return;
	}
	FVector	SpawnPos = InLocation + this->SpaceShipActor__->GetActorLocation();
	FVector	ItemMoveDir = -InLocation;
	int32	TryCount = 0;

	ItemMoveDir.Normalize();
	while (TryCount < this->ItemSpawnMaxRetryCount__)
	{
		++TryCount;

		FVector	TempDir = ItemMoveDir;
		TempDir.X += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Y += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Z += FMath::FRandRange(-0.3f, 0.3f);
		TempDir.Normalize();
		float	DistSquared = FUtilFunction::GetPointToLineDistanceSquared(
			this->SpaceShipActor__->GetActorLocation(),
			SpawnPos,
			TempDir
		);
		if (DistSquared > this->SafeAreaSquared__)
		{
			ItemMoveDir = TempDir;
			break;
		}

		if (TryCount == this->ItemSpawnMaxRetryCount__)
		{
			// 안전한 방향을 찾지 못하면 이번 스폰은 건너뜀
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] 안전한 아이템 속도를 찾지 못했습니다."));
			return;
		}
	}
	float							ItemSpeed = this->ItemMoveSpeed__ * FMath::FRandRange(0.8, 1.2);
	FVector							Velocity = ItemMoveDir * ItemSpeed;
	TWeakObjectPtr<USceneComponent> WeakPivot(ItemPivot);
	UItemDataAsset*					TargetItemData = this->SelectSpawnItemData__();
	FTransform						WorldSpawnTransform(FRotator::ZeroRotator, SpawnPos);

	ItemFactory->SpawnItemActorAsync(
		TargetItemData,
		WorldSpawnTransform,
		FOnPickupSpawned::CreateWeakLambda(
			this,
			[this, WeakPivot, Velocity, Generation = SpawnGeneration__](AItemActor* ItemActor)
			{
				if (!IsValid(ItemActor))
				{
					UE_LOG(
						LogTemp,
						Error,
						TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] ItemActor가 스폰되지 않았습니다.")
					);
					return;
				}
				if (!bSpawningEnabled__ || Generation != SpawnGeneration__ || !WeakPivot.IsValid())
				{
					UE_LOG(
						LogTemp,
						Error,
						TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] WeakPivot이 Valid하지 않습니다.")
					);
					ItemActor->FinishUsingPoolable();
					return;
				}
				ItemActor->AttachToComponent(
					WeakPivot.Get(),
					FAttachmentTransformRules::KeepWorldTransform
				);
				ItemActor->SetRelativeVelocity(Velocity);
				this->SpawnedItem__.AddUnique(ItemActor);
				UE_LOG(
					LogTemp,
					Log,
					TEXT("[USpaceSalvageWorldSubsystem::SpawnItemActor__] 아이템 %s 생성 위치: %s"),
					*ItemActor->GetName(),
					*ItemActor->GetActorLocation().ToString()
				);
			}), [WeakThis = TWeakObjectPtr<USpaceSalvageWorldSubsystem>(this), Generation = SpawnGeneration__]()
		{
			return WeakThis.IsValid() && WeakThis->bSpawningEnabled__ && WeakThis->SpawnGeneration__ == Generation;
		}
	);
}

void USpaceSalvageWorldSubsystem::DespawnItemActor__()
{
	for (int32 Index = SpawnedItem__.Num() - 1; Index >= 0; --Index)
	{
		AItemActor*	TargetItem = SpawnedItem__[Index].Get();

		if (!IsValid(TargetItem))
		{
			SpawnedItem__.RemoveAtSwap(Index);
			continue;
		}
		float	ItemDistance = FVector::DistSquared(TargetItem->GetActorLocation(), this->SpaceRootActor__->GetActorLocation());

		if (ItemDistance > this->ItemDespawnDistSquared__)
		{
			SpawnedItem__.RemoveAtSwap(Index);
			TargetItem->FinishUsingPoolable();
			UE_LOG(
				LogTemp, 
				Log, 
				TEXT("[USpaceSalvageWorldSubsystem::DespawnItemActor__] 아이템 %s가 Return됐습니다."), 
				*TargetItem->GetName()
			);
		}
	}
}

bool USpaceSalvageWorldSubsystem::HasPendingMeteor() const
{
	return bMeteorLoading__ || (ActiveMeteor__.IsValid() && ActiveMeteor__->IsMeteorActive()) || (IsValid(SpaceShipActor__) && SpaceShipActor__->GetMeteorAvoidance() && SpaceShipActor__->GetMeteorAvoidance()->HasMeteor());
}

void USpaceSalvageWorldSubsystem::StopSurvival()
{
	EndOfDay();
	++SpawnGeneration__;
	bMeteorLoading__ = false;
	GetWorld()->GetTimerManager().ClearTimer(ItemDespawnHandler__);
	if (ActiveMeteor__.IsValid() && ActiveMeteor__->IsMeteorActive()) { ActiveMeteor__->FinishUsingPoolable(); }
	ActiveMeteor__.Reset();
}

void USpaceSalvageWorldSubsystem::SpawnMeteor__(const FMeteor& InMeteor)
{
	auto* Factory = GetWorld()->GetSubsystem<UItemActorFactorySubsystem>();
	if (!Factory || !IsValid(SpaceMapData__) || !IsValid(SpaceShipActor__) || ItemSpawnDist__ <= 0.0f) { return; }
	const FVector Center = SpaceShipActor__->GetActorLocation();
	const float Distance = ItemSpawnDist__;
	// MeteorAvoidance already stores StartPos relative to the ship. Applying an
	// extra two spawn distances put the physical actor three times too far away.
	const FTransform Transform(InMeteor.MoveDir.Rotation(), Center + InMeteor.StartPos);
	bMeteorLoading__ = true;
	Factory->SpawnItemActorAsync(SpaceMapData__->MeteorData, Transform, FOnPickupSpawned::CreateWeakLambda(this, [this, InMeteor, Center, Distance, Generation = SpawnGeneration__](AItemActor* Item)
	{
		if (Generation != SpawnGeneration__) { if (IsValid(Item)) { Item->FinishUsingPoolable(); } return; }
		bMeteorLoading__ = false;
		auto* Meteor = Cast<AMeteorItemActor>(Item);
		if (!Meteor || !IsValid(SpaceShipActor__)) { if (IsValid(Item)) { Item->FinishUsingPoolable(); } return; }
		FMeteor Prepared = InMeteor;
		if (Prepared.MeteorDamage <= 0.0f) { Meteor->FinishUsingPoolable(); return; }
		ActiveMeteor__ = Meteor;
		Meteor->SetRelativeVelocity(Prepared.MoveDir * Prepared.MeteorSpeed);
		Meteor->InitMeteor(Prepared, Center, Distance);
		SpaceShipActor__->GetLazerComponent()->AttackMeteo__(Meteor);
		if (Meteor->IsMeteorActive()) { OnMeteorSpawn.ExecuteIfBound(Meteor); }
	}));
}
UItemDataAsset* USpaceSalvageWorldSubsystem::SelectSpawnItemData__()
{
	if (!this->SpaceMapData__)
	{
		UE_LOG(
			LogTemp, 
			Error,
			TEXT("[USpaceSalvageWorldSubsystem::SelectSpawnItemData__] ItemSpawnRateData가 nullptr입니다.")
		);
		return (nullptr);
	}
	int32	TotalWeight = 0;

	for (auto& RateData : this->SpaceMapData__->ItemSpawnRate)
	{
		TotalWeight += RateData.Value;
	}
	if (TotalWeight <= 0)
	{
		UE_LOG(
			LogTemp, 
			Warning, 
			TEXT("[USpaceSalvageWorldSubsystem::SelectSpawnItemData__] TotalWeight가 0 이하입니다.")
		);
		return (nullptr);
	}
	int32	RandomWeight = FMath::RandRange(1, TotalWeight);

	for (auto& RateData : this->SpaceMapData__->ItemSpawnRate)
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
	UE_LOG(
		LogTemp, 
		Error, 
		TEXT("[USpaceSalvageWorldSubsystem::SelectSpawnItemData__] 아이템이 생성되지 않았습니다.")
	);
	return (nullptr);
}

void USpaceSalvageWorldSubsystem::CheckStartDay__()
{
	if (bStartCheckScheduled__ || !IsValid(SpaceShipActor__) || !IsValid(PlayerCharacter__)
		|| !IsValid(SurvivalLoop__) || !SurvivalLoop__->bAutoStart
		|| SurvivalLoop__->State != ESurvivalState::Ready)
	{
		return;
	}

	// Actor/component and Blueprint BeginPlay order is not guaranteed across actors.
	// Defer once after all three participants have announced that their own setup is done.
	bStartCheckScheduled__ = true;
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		bStartCheckScheduled__ = false;
		if (!IsValid(SpaceShipActor__) || !IsValid(PlayerCharacter__) || !IsValid(SurvivalLoop__)
			|| !SurvivalLoop__->bAutoStart || SurvivalLoop__->State != ESurvivalState::Ready)
		{
			return;
		}

		if (!IsValid(SurvivalLoop__->SpaceShip)) SurvivalLoop__->SpaceShip = SpaceShipActor__;
		if (!IsValid(SurvivalLoop__->Player)) SurvivalLoop__->Player = PlayerCharacter__;
		if (!SurvivalLoop__->StartSurvival())
		{
			UE_LOG(LogTemp, Error, TEXT("Auto StartSurvival failed after player, ship, and loop initialization completed."));
		}
	}));
}
