// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ItemActorFactorySubsystem.h"
#include "Data/Item/ItemDataAsset.h"
#include "Item/ItemActor.h"

bool UItemActorFactorySubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (!Super::ShouldCreateSubsystem(Outer)) return false;

    //const UWorld* World = Cast<UWorld>(Outer);
    //World->IsGameWorld();
    //World->IsEditorWorld();
    //World->WorldType == EWorldType::Game;
    return true;
}

void UItemActorFactorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("[UItemActorFactorySubsystem::Initialize()] : Initialized"));
}

void UItemActorFactorySubsystem::Deinitialize()
{
    // 진행중인 비동기 로딩 핸들 정리(월드가 종료될 경우에는 콜백을 호출하지 않음)
    for (TSharedPtr<FStreamableHandle>& Handle : ActiveStreamableHandles__)
    {
        if (Handle.IsValid() && Handle->IsActive())
        {
            Handle->CancelHandle();
        }
    }

    ActiveStreamableHandles__.Empty();
    UE_LOG(LogTemp, Log, TEXT("[UItemActorFactorySubsystem::Deinitialize()] : Deinitialized"));

    Super::Deinitialize();
}

AItemActor* UItemActorFactorySubsystem::SpawnItemActor(const UItemDataAsset* InItemData, const FTransform& InTransform)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnItemActor()] : InItemData가 nullptr입니다."));
        return nullptr;
    }

    if (!InItemData->IsLoaded())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnItemActor()] : %s의 SoftPtr이 모두 로드되지 않았습니다."),
               *InItemData->DisplayName.ToString());
        return nullptr;
    }

    return SpawnProcess__(InItemData, InTransform);
}

void UItemActorFactorySubsystem::SpawnItemActorAsync(const UItemDataAsset* InItemData, const FTransform& InTransform, FOnPickupSpawned OnSpawned)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnItemActorAsync()] : InItemData가 nullptr입니다."));
        OnSpawned.ExecuteIfBound(nullptr);
        return;
    }

    // 아이템 데이터가 이미 로딩된 상태면 즉시 스폰
    if (InItemData->IsLoaded())
    {
        AItemActor* Spawned = SpawnProcess__(InItemData, InTransform);
        OnSpawned.ExecuteIfBound(Spawned);
        return;
    }

    // 아이템 데이터가 아직 로딩되지 않았다면 비동기 로딩 요청
    TWeakObjectPtr<const UItemDataAsset> WeakItemData(InItemData);
    TSharedPtr<FStreamableHandle> Handle = InItemData->RequestDataLoad(
        FStreamableDelegate::CreateWeakLambda(
            this,
            [this, WeakItemData, InTransform, OnSpawned]() {
                if (!WeakItemData.IsValid())
                {
                    UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnItemActorAsync()] : 비동기 로딩 요청 중 InItemData가 유효하지 않습니다."));
                    OnSpawned.ExecuteIfBound(nullptr);
                    CleanupCompletedHandles__();
                    return;
                }

                const UItemDataAsset* LoadedItemData = WeakItemData.Get();
                AItemActor* Spawned = SpawnProcess__(LoadedItemData, InTransform);

                OnSpawned.ExecuteIfBound(Spawned);
                CleanupCompletedHandles__();

                UE_LOG(LogTemp, Log, TEXT("[UItemActorFactorySubsystem::SpawnItemActorAsync()] : 비동기 스폰 완료 (%s)"),
                       *LoadedItemData->DisplayName.ToString());
            }
        )
    );

    if (Handle.IsValid())
    {
        ActiveStreamableHandles__.Add(Handle);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnItemActorAsync()] : 비동기 로딩 요청에 실패했습니다."));
        OnSpawned.ExecuteIfBound(nullptr);
    }
}

void UItemActorFactorySubsystem::K2_SpawnItemActorAsync(const UItemDataAsset* InItemData, const FTransform& InTransform, FOnPickupSpawnedDynamic OnSpawned)
{
    SpawnItemActorAsync(
        InItemData,
        InTransform,
        FOnPickupSpawned::CreateLambda(
            [OnSpawned](AItemActor* SpawnedPickup) {
                OnSpawned.ExecuteIfBound(SpawnedPickup);
            }
        )
    );
}

AItemActor* UItemActorFactorySubsystem::SpawnProcess__(const UItemDataAsset* InItemData, const FTransform& InTransform)
{
    UWorld* World = GetWorld();

    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnProcess__()] : World가 없습니다."));
        return nullptr;
    }

    UClass* ItemActorClass = InItemData->ItemActorClass.Get();

    if (!ItemActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UItemActorFactorySubsystem::SpawnProcess__()] : %s의 ItemActorClass가 없습니다."),
               *InItemData->DisplayName.ToString());
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AItemActor* Spawned = World->SpawnActor<AItemActor>(ItemActorClass, InTransform, SpawnParams);

    if (Spawned)
    {
        Spawned->InitializeItemActor(InItemData);
        UE_LOG(LogTemp, Log, TEXT("[UItemActorFactorySubsystem::SpawnProcess__()] : %s를 %s 위치에 스폰했습니다."),
               *InItemData->DisplayName.ToString(), *InTransform.GetLocation().ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[UItemActorFactorySubsystem::SpawnProcess__()] : %s를 스폰하는데 실패했습니다."),
               *InItemData->DisplayName.ToString());
    }

    return Spawned;
}

void UItemActorFactorySubsystem::CleanupCompletedHandles__()
{
    ActiveStreamableHandles__.RemoveAll(
        [](const TSharedPtr<FStreamableHandle>& Handle) {
            return !Handle.IsValid() || Handle->HasLoadCompleted();
        }
    );
}
