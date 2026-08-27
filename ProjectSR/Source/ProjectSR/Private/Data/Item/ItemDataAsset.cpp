// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/ItemDataAsset.h"
#include "Engine/AssetManager.h"
//#include "Item/ItemActor.h"
//#include "Data/ItemAction/ItemAction.h"

//TSharedPtr<FStreamableHandle> UItemDataAsset::RequestDataLoad(FStreamableDelegate InDelegate) const
//{
//    TArray<FSoftObjectPath> TargetsToLoad;
//    OnAsyncRequest_(TargetsToLoad);
//
//    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
//    return StreamableManager.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
//}
//
//bool UItemDataAsset::IsLoaded() const
//{
//    return ItemActorClass.IsValid() && Mesh.IsValid() && (Icon.IsNull() || Icon.IsValid());
//}
//
//void UItemDataAsset::OnAsyncRequest_(TArray<FSoftObjectPath>& InOutArray) const
//{
//    InOutArray.Add(ItemActorClass.ToSoftObjectPath());
//    InOutArray.Add(Icon.ToSoftObjectPath());
//    InOutArray.Add(Mesh.ToSoftObjectPath());
//}
