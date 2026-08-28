// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/StreamableManager.h"
#include "ItemActorFactorySubsystem.generated.h"

class AItemActor;
class UItemDataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPickupSpawnedDynamic, AItemActor*, SpawnedPickup);
DECLARE_DELEGATE_OneParam(FOnPickupSpawned, AItemActor*);

UCLASS()
class PROJECTSR_API UItemActorFactorySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:

    // USubSystem 함수 오버라이드 ------------------------------------------------------------
    // 서브시스템을 만들지 여부를 결정하는 함수
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    // 서브시스템이 생성되었을 때 실행될 함수
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 서브시스템이 삭제 될 때 실행될 함수
    virtual void Deinitialize() override;
    //-------------------------------------------------------------------------------------

    // 아이템 액터 스폰(동기방식)
    UFUNCTION(BlueprintCallable, Category = "Factory|Pickup")
    AItemActor* SpawnItemActor(const UItemDataAsset* InItemData, const FTransform& InTransform);

    // 아이템 액터 스폰(비동기 방식, 로딩 완료 후 OnSpawned 델리게이트 호출)
    void SpawnItemActorAsync(const UItemDataAsset* InItemData, const FTransform& InTransform, FOnPickupSpawned OnSpawned);

    UFUNCTION(BlueprintCallable, Category = "Factory|Pickup", meta = (DisplayName = "Spawn Pickup Async"))
    void K2_SpawnItemActorAsync(const UItemDataAsset* InItemData, const FTransform& InTransform, FOnPickupSpawnedDynamic OnSpawned);

private:
    // 실제 스폰을 처리할 함수
    AItemActor* SpawnProcess__(const UItemDataAsset* InItemData, const FTransform& InTransform);

    // 비동기 작업의 핸들 중 완료된 핸들을 정리하는 함수
    void CleanupCompletedHandles__();

private:
    // 진행중인 비동기 작업의 핸들 모음
    TArray<TSharedPtr<FStreamableHandle>> ActiveStreamableHandles__;
};
