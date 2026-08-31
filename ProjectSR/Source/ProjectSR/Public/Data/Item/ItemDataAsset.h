// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "ItemDataAsset.generated.h"

class AItemActor;
class UItemAction;

UENUM()
enum class EItemType
{
    None,
    Resource,
    Usable,
    Equipment
};

UCLASS(BlueprintType)
class PROJECTSR_API UItemDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    TSharedPtr<FStreamableHandle> RequestDataLoad(FStreamableDelegate InDelegate) const;
    virtual bool IsLoaded() const;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
protected:
    virtual void OnAsyncRequest_(TArray<FSoftObjectPath>& InOutArray) const;

public:
    // 아이템 ID
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    FName ItemId = NAME_None;

    // 아이템 타입
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    EItemType ItemType = EItemType::None;

    // 아이템 이름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    FText DisplayName = FText::GetEmpty();

    // 아이템 설명
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    FText Description = FText::GetEmpty();

    // 아이템 무게
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    float Weight = 0.1f;

    // 인벤토리 한 칸에 쌀일 수 있는 최대 개수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    int32 MaxStackCount = 1;

    // 스폰할 아이템 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    TSoftClassPtr<AItemActor> ItemActorClass = nullptr;

    // UI에서 보여줄 아이템 아이콘 이미지
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    TSoftObjectPtr<UTexture2D> Icon = nullptr;

    // 아이템 스태틱 메시
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    TSoftObjectPtr<UStaticMesh> Mesh = nullptr;

    // 아이템 사용 시 실행될 액션
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ItemData")
    TObjectPtr<UItemAction> ItemAction = nullptr;

};
