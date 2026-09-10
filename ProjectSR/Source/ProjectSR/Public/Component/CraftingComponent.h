// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommonHeader/RecipeTable.h"
#include "CraftingComponent.generated.h"

class UInventoryComponent;
class UItemDataAsset;
struct FInventorySlot;

USTRUCT(BlueprintType)
struct FRecipeEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName RecipeId;

    UPROPERTY(BlueprintReadOnly)
    FName RecipeName;

    UPROPERTY(BlueprintReadOnly)
    FRecipeTableRow RecipeData;

};

USTRUCT(BlueprintType)
struct FManufactureWidgetDisplayData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FRecipeEntry RecipeEntry;

    UPROPERTY(BlueprintReadOnly)
    TMap<FName, int32> IngredientStatusMap;

    UPROPERTY(BlueprintReadOnly)
    bool bHasEnoughIngredients;

    UPROPERTY(BlueprintReadOnly)
    bool bHasEnoughEmptySlots;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManufactureWidgetOpened, const FManufactureWidgetDisplayData&, InData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCraftFailed);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSR_API UCraftingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCraftingComponent();

    // 제작법을 해금하는 함수
    void Unlock(FName InRecipeId);

    bool HasRecipe(FName RecipeId) const;

    // 제작법이 해금됐는지 여부를 반환하는 함수
    bool IsUnlockedRecipe(FName InRecipeId, bool bDefaultLocked) const;

    // 제작법의 재료 아이템이 인벤토리(들)에 충분히 존재하는지 확인하는 함수
    bool HasEnoughIngredients(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories) const;

    // 제작법의 결과 아이템들이 들어갈 공간이 인벤토리(들)에 충분히 존재하는지 확인하는 함수
    bool HasEnoughEmptySlots(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories) const;

    bool Craft(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories);

    FRecipeEntry GetRecipeEntry(FName InRecipeId) const;

    TArray<FRecipeEntry> GetUnlockedRecipeEntries() const;

    FManufactureWidgetDisplayData BuildManufactureWidgetDisplayData(FName InRecipeId) const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // InItemData 아이템의 개수가 가장 적은 슬롯의 인덱스를 반환하는 함수
    int32 FindSlotWithFewestCount__(const TArray<FInventorySlot>& InSlots, const UItemDataAsset* InItemData, int32 StartIndex) const;

    // InItemData 아이템을 가진 슬롯의 인덱스를 반환하는 함수
    int32 FindSlotWithItem__(const TArray<FInventorySlot>& InSlots, const UItemDataAsset* InItemData, int32 StartIndex) const;

    // 빈 슬롯의 인덱스를 반환하는 함수
    int32 FindEmptySlot__(const TArray<FInventorySlot>& InSlots) const;

public:
    void HandleRecipeSelected(FName InRecipeId);
    void HandleCraftRequested(FName InRecipeId);

public:
    FOnManufactureWidgetOpened OnManufactureWidgetOpened;
    FOnCraftFailed OnCraftFailed;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting|RecipeTable")
    TObjectPtr<UDataTable> RecipeTable_;

private:
    TSet<FName> UnlockedRecipeIds__;

};
