// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CommonHeader/ObjectPoolEnums.h"
#include "ObjectPoolSubsystem.generated.h"

class UObjectPoolDataAsset;
using FActiveActorNode = TDoubleLinkedList<TObjectPtr<AActor>>::TDoubleLinkedListNode;

USTRUCT()
struct FObjectPool
{
    GENERATED_BODY()

public:
    FObjectPool() :
        ActiveActorOrderedList(MakeShared<TDoubleLinkedList<TObjectPtr<AActor>>>()),
        ActiveActorNodeMap(MakeShared<TMap<TObjectPtr<AActor>, FActiveActorNode*>>())
    {
    }

    // 사용 대기 중인 액터들
    UPROPERTY(Transient)
    TArray<TObjectPtr<AActor>> ReadyActors;

    // 실제 사용 중인 액터들
    UPROPERTY(Transient)
    TSet<TObjectPtr<AActor>> ActiveActors;

    // 사용 순서를 기록할 더블 링크드 리스트(Head가 가장 오래됨, Tail이 가장 새것) (주의:GC가 추적은 못함)
    TSharedPtr<TDoubleLinkedList<TObjectPtr<AActor>>> ActiveActorOrderedList;

    // 액터 포인터를 키값으로 하고, ActiveActorOrderedList의 노드 주소를 Value로 하는 맵
    TSharedPtr<TMap<TObjectPtr<AActor>, FActiveActorNode*>> ActiveActorNodeMap;

    // 초기 생성 개수
    UPROPERTY(Transient)
    int32 InitialSize = 0;

    // 최대 관리 개수
    UPROPERTY(Transient)
    int32 MaxSize = 0;

    // 최대치 도달 시 정책
    UPROPERTY(Transient)
    EObjectPoolPolicy MaxPolicy = EObjectPoolPolicy::Grow;

};

UCLASS()
class PROJECTSR_API UObjectPoolSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    bool RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup = false);

    UFUNCTION(BlueprintCallable)
    bool UnregisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset);

    UFUNCTION(BlueprintCallable)
    void Warmup(const TSubclassOf<AActor> InClass);

    UFUNCTION(BlueprintCallable)
    void WarmupAll();

    UFUNCTION(BlueprintCallable)
    void ClearPool(const TSubclassOf<AActor> InClass);

    UFUNCTION(BlueprintCallable)
    void ClearAllPools();

    UFUNCTION(BlueprintCallable)
    AActor* SpawnFromPool(TSubclassOf<AActor> InClassType, const FTransform& InTransform);

    template<typename T>
    T* SpawnFromPool(TSubclassOf<T> InClassType, const FTransform& InTransform)
    {
        return Cast<T>(SpawnFromPool(TSubclassOf<AActor>(InClassType), InTransform));
    }

    UFUNCTION(BlueprintCallable)
    void ReturnToPool(AActor* InActor);

protected:
    AActor* CreateNewObject_(const TSubclassOf<AActor> InClassType, const FTransform& InTransform);
    AActor* GetReadyActor_(FObjectPool* InPool);

protected:
    UPROPERTY(Transient)
    TMap<const TSubclassOf<AActor>, FObjectPool> ObjectPools_;

};
