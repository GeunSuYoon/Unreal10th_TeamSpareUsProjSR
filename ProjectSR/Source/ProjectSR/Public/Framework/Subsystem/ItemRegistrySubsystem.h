// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "ItemRegistrySubsystem.generated.h"

class UItemDataAsset;
struct FPrimaryAssetId;

UCLASS()
class PROJECTSR_API UItemRegistrySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UItemDataAsset* GetItemDataAsset(FPrimaryAssetId InItemId) const;

private:
    TSharedPtr<FStreamableHandle> LoadHandle;
    TMap<FPrimaryAssetId, TObjectPtr<UItemDataAsset>> LoadedItems;
};
