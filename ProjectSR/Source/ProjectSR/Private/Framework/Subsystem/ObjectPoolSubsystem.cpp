// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Data/ObjectPool/ObjectPoolDataAsset.h"
#include "Interface/PoolableInterface.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 프로젝트 세팅에서 데이터 읽어오기
    const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();

    if (!Settings)
    {
        UE_LOG(LogTemp, Error, TEXT("[UObjectPoolSubsystem::Initialize()] : Settings 불러오기 오류!"));
        return;
    }

    // 세팅에 있는 풀 데이터를 로딩한 후
    // 해당 데이터를 기반으로 풀을 만들어 ObjectPools_에 추가
    for (const TSoftObjectPtr<UObjectPoolDataAsset>& DataAsset : Settings->PoolDataAssets)
    {
        if (!DataAsset.IsNull())
        {
            UObjectPoolDataAsset* LoadedDataAsset = DataAsset.LoadSynchronous();
            FObjectPool& Pool = ObjectPools_.FindOrAdd(LoadedDataAsset->ActorClass.LoadSynchronous());
            Pool.InitialSize = LoadedDataAsset->InitialSize;
            Pool.MaxSize = LoadedDataAsset->MaxSize;
            Pool.MaxPolicy = LoadedDataAsset->MaxPolicy;
        }
    }
}

void UObjectPoolSubsystem::Deinitialize()
{
    ClearAllPools();

    Super::Deinitialize();
}

bool UObjectPoolSubsystem::RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup)
{
    if (!InDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::RegisterPoolDataAsset()] : ObjectPoolDataAsset이 nullptr 입니다."));
        return false;
    }

    if (InDataAsset->ActorClass.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::RegisterPoolDataAsset()] : ObjectPoolDataAsset의 ActorClass가 설정되지 않았습니다."));
        return false;
    }

    const TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();

    // 이미 있는 풀이라면 정리
    ClearPool(LoadedActorClass);

    FObjectPool& Pool = ObjectPools_.Add(LoadedActorClass);
    Pool.InitialSize = InDataAsset->InitialSize;
    Pool.MaxSize = InDataAsset->MaxSize;
    Pool.MaxPolicy = InDataAsset->MaxPolicy;

    // 웜업 요청이 있으면 웜업
    if (bWarmup)
    {
        Warmup(LoadedActorClass);
    }

    return true;
}

bool UObjectPoolSubsystem::UnregisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset)
{
    if (!InDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::UnregisterPoolDataAsset()] : ObjectPoolDataAsset이 nullptr 입니다."));
        return false;
    }

    if (InDataAsset->ActorClass.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::UnregisterPoolDataAsset()] : ObjectPoolDataAsset의 ActorClass가 설정되지 않았습니다."));
        return false;
    }

    TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();

    ClearPool(LoadedActorClass);

    return true;
}

void UObjectPoolSubsystem::Warmup(const TSubclassOf<AActor> InClass)
{
    FObjectPool* Pool = ObjectPools_.Find(InClass);

    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::Warmup()] : 풀 목록에 존재하지 않는 InClass입니다."));
        return;
    }

    //FTransform Init(FVector::DownVector * 10000.0f);
    FTransform Init(FVector(1000.0f, 0.0f, 1000.0f));

    for (int _ = 0; _ < Pool->InitialSize; _++)
    {
        AActor* Spawned = CreateNewObject_(InClass, Init);

        if (Spawned)
        {
            UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::Warmup()] : %s"),
                   *Spawned->GetName());

            if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
            {
                IPoolableInterface::Execute_OnReturnToPool(Spawned);
            }
            else
            {
                Spawned->SetActorHiddenInGame(true);
                Spawned->SetActorTickEnabled(false);
                Spawned->SetActorEnableCollision(false);
            }

            Pool->ReadyActors.Add(Spawned);
        }
    }
}

void UObjectPoolSubsystem::WarmupAll()
{
    for (auto& [Key, _] : ObjectPools_)
    {
        Warmup(Key);
    }
}

void UObjectPoolSubsystem::ClearPool(const TSubclassOf<AActor> InClass)
{
    FObjectPool* Pool = ObjectPools_.Find(InClass);

    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::ClearPool()] : 풀 목록에 존재하지 않는 InClass입니다."));
        return;
    }

    for (AActor* Actor : Pool->ReadyActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Pool->ReadyActors.Empty();

    for (AActor* Actor : Pool->ActiveActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Pool->ActiveActors.Empty();

    Pool->ActiveActorOrderedList->Empty();
    Pool->ActiveActorNodeMap->Empty();

    ObjectPools_.Remove(InClass);
}

void UObjectPoolSubsystem::ClearAllPools()
{
    for (auto& [_, Pool] : ObjectPools_)
    {
        for (AActor* Actor : Pool.ReadyActors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.ReadyActors.Empty();

        for (AActor* Actor : Pool.ActiveActors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.ActiveActors.Empty();

        Pool.ActiveActorOrderedList->Empty();
        Pool.ActiveActorNodeMap->Empty();
    }

    ObjectPools_.Empty();
}

AActor* UObjectPoolSubsystem::SpawnFromPool(const TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
    if (!InClassType)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : InClassType 이 nullptr 입니다."));
        return nullptr;
    }

    FObjectPool* Pool = ObjectPools_.Find(InClassType);

    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : 풀 목록에 %s 이 없습니다."),
               *InClassType->GetName());
        return nullptr;
    }

    AActor* Spawned = nullptr;

    // ReadyActors에 쓸 수 있는 게 있으면 가져옴
    Spawned = GetReadyActor_(Pool);

    if (Spawned)
    {
        Spawned->SetActorTransform(InTransform);

        UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : Spawn(Reuse) %s"),
               *Spawned->GetName());
    }
    // ReadyActors에서 쓸 수 있는 게 없으면 새로 만들기
    else
    {
        const int32 TotalCount = Pool->ActiveActors.Num() + Pool->ReadyActors.Num();
        const bool bMax = TotalCount >= Pool->MaxSize;

        if (!bMax)
        {
            Spawned = CreateNewObject_(InClassType, InTransform);

            UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : Spawn(New) %s"),
                   Spawned ? *Spawned->GetName() : TEXT("None"));
        }
        else
        {
            switch (Pool->MaxPolicy)
            {
                case EObjectPoolPolicy::Grow:
                    Spawned = CreateNewObject_(InClassType, InTransform);

                    UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : Spawn(New) %s"),
                           Spawned ? *Spawned->GetName() : TEXT("None"));
                    break;
                case EObjectPoolPolicy::ReuseOldest:
                    if (FActiveActorNode* HeadNode = Pool->ActiveActorOrderedList->GetHead())
                    {
                        AActor* OldestActor = HeadNode->GetValue();

                        ReturnToPool(OldestActor);

                        Spawned = GetReadyActor_(Pool);
                        Spawned->SetActorTransform(InTransform);

                        UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::SpawnFromPool()] : Spawn(Oldest) %s"),
                               Spawned ? *Spawned->GetName() : TEXT("None"));;
                    }
                    break;
                case EObjectPoolPolicy::DoNotSpawn:
                default:
                    return nullptr;
            }
        }
    }

    if (Spawned)
    {
        if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
        {
            IPoolableInterface::Execute_OnSpawnFromPool(Spawned);
        }
        else
        {
            Spawned->SetActorHiddenInGame(false);
            Spawned->SetActorTickEnabled(true);
            Spawned->SetActorEnableCollision(true);
        }

        Pool->ActiveActors.Add(Spawned);

        FActiveActorNode* NewNode = new FActiveActorNode(Spawned);
        Pool->ActiveActorOrderedList->AddTail(NewNode);
        Pool->ActiveActorNodeMap->Add(Spawned, NewNode);
    }

    return Spawned;
}

void UObjectPoolSubsystem::ReturnToPool(AActor* InActor)
{
    if (!InActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::ReturnToPool()] : InActor가 nullptr입니다."));
        return;
    }

    const TSubclassOf<AActor> ActorClass = InActor->GetClass();
    FObjectPool* Pool = ObjectPools_.Find(ActorClass);

    if (!Pool)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::ReturnToPool()] : 풀 목록에 InActor 풀이 없습니다."));
        return;
    }

    if (!Pool->ActiveActors.Contains(InActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::ReturnToPool()] : 풀에 InActor가 없습니다."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[UObjectPoolSubsystem::ReturnToPool()] : Return %s"),
           *InActor->GetName());

    if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
    {
        IPoolableInterface::Execute_OnReturnToPool(InActor);
    }
    else
    {
        InActor->SetActorHiddenInGame(true);
        InActor->SetActorTickEnabled(false);
        InActor->SetActorEnableCollision(false);
    }

    Pool->ActiveActors.Remove(InActor);
    Pool->ReadyActors.Add(InActor);

    // 리스트, 맵에서 InActor 노드를 제거
    if (FActiveActorNode** FoundNode = Pool->ActiveActorNodeMap->Find(InActor))
    {
        Pool->ActiveActorOrderedList->RemoveNode(*FoundNode, true);
        Pool->ActiveActorNodeMap->Remove(InActor);
    }
}

AActor* UObjectPoolSubsystem::CreateNewObject_(const TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
    UWorld* World = GetWorld();

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[UObjectPoolSubsystem::CreateNewObject_()] : World가 nullptr입니다!"));
        return nullptr;
    }

    FActorSpawnParameters SpawnParam;
    SpawnParam.Owner = nullptr;
    SpawnParam.ObjectFlags = RF_Transient;

    AActor* Spawned = World->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);

#if WITH_EDITOR
    if (Spawned)
    {
        Spawned->SetFolderPath(FName("Pool"));
    }
#endif

    return Spawned;
}

AActor* UObjectPoolSubsystem::GetReadyActor_(FObjectPool* InPool)
{
    if (!InPool)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UObjectPoolSubsystem::GetReadyActor_()] : InPool이 nullptr입니다."));
        return nullptr;
    }

    AActor* ReadyActor = nullptr;

    while (InPool->ReadyActors.Num() > 0)
    {
        AActor* Candidate = InPool->ReadyActors.Pop();
        if (IsValid(Candidate))
        {
            ReadyActor = Candidate;
            break;
        }
    }
    return ReadyActor;
}
