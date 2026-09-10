// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ItemRegistrySubsystem.h"
#include "Data/Item/ItemDataAsset.h"

#include "Engine/AssetManager.h"

void UItemRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    /*
    UAssetManager& Manager = UAssetManager::Get();
    TArray<FPrimaryAssetId> ItemIds;
    Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("Item")), ItemIds);

    LoadHandle = Manager.LoadPrimaryAssets(
        ItemIds,
        TArray<FName>(),
        FStreamableDelegate::CreateLambda(
            [this, ItemIds]() {
                for (const FPrimaryAssetId& Id : ItemIds)
                {
                    if (UItemDataAsset* Asset = Cast<UItemDataAsset>(
                        UAssetManager::Get().GetPrimaryAssetObject(Id)))
                    {
                        LoadedItems.Add(Id, Asset);
                    }
                }
            })
    );
    */
}

UItemDataAsset* UItemRegistrySubsystem::GetItemDataAsset(FPrimaryAssetId InItemId) const
{
    return nullptr;
}
