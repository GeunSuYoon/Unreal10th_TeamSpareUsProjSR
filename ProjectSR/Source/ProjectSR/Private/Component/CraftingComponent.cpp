// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CraftingComponent.h"
#include "Component/InventoryComponent.h"
#include "CommonHeader/RecipeTable.h"
#include "CommonHeader/InventoryCommandTypes.h"
#include "Data/Item/ItemDataAsset.h"

UCraftingComponent::UCraftingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCraftingComponent::Unlock(FName InRecipeId)
{
    if (!RecipeTable_)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::Unlock()] : RecipeTable 데이터 테이블 에셋이 설정되지 않았습니다."));
        return;
    }

    const FRecipeTableRow* Recipe = RecipeTable_->FindRow<FRecipeTableRow>(InRecipeId, TEXT("UCraftingComponent::Unlock()"));

    if (!Recipe)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::Unlock()] : 존재하지 않는 제작법입니다 (%s)"),
               *InRecipeId.ToString());
        return;
    }

    UnlockedRecipeIds__.Add(InRecipeId);
}

bool UCraftingComponent::IsUnlockedRecipe(FName InRecipeId, bool bDefaultLocked) const
{
    return !bDefaultLocked || UnlockedRecipeIds__.Contains(InRecipeId);
}

bool UCraftingComponent::HasEnoughIngredients(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories) const
{
    const FRecipeTableRow* Recipe = RecipeTable_->FindRow<FRecipeTableRow>(InRecipeId, TEXT("UCraftingComponent::HasEnoughIngredients()"));

    if (!Recipe)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughIngredients()] : 존재하지 않는 제작법입니다 (%s)"),
               *InRecipeId.ToString());
        return false;
    }

    if (!IsUnlockedRecipe(InRecipeId, Recipe->bLockedByDefault))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughIngredients()] : %s 제작법은 아직 해금되지 않았습니다."),
               *InRecipeId.ToString());
        return false;
    }

    // 제작 재료 충족 여부 확인
    for (const FIngredient& Ingredient : Recipe->Ingredients)
    {
        const UItemDataAsset* ItemData = Ingredient.ItemData;
        int32 TotalItemCount = 0;

        for (UInventoryComponent* Inventory : InInventories)
        {
            TotalItemCount += Inventory->GetTotalItemCount(ItemData);
        }

        if (TotalItemCount < Ingredient.Quantity)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughIngredients()] : %s 제작 재료가 부족합니다."),
                   *InRecipeId.ToString());
            return false;
        }
    }

    return true;
}

bool UCraftingComponent::HasEnoughEmptySlots(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories) const
{
    const FRecipeTableRow* Recipe = RecipeTable_->FindRow<FRecipeTableRow>(InRecipeId, TEXT("UCraftingComponent::HasEnoughEmptySlots()"));

    if (!Recipe)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : 존재하지 않는 제작법입니다 (%s)"),
               *InRecipeId.ToString());
        return false;
    }

    if (!IsUnlockedRecipe(InRecipeId, Recipe->bLockedByDefault))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : %s 제작법은 아직 해금되지 않았습니다."),
               *InRecipeId.ToString());
        return false;
    }

    // 제작 결과물이 들어갈 인벤토리 공간 여부 확인
    TArray<TArray<FInventorySlot>> CopiedInventories;
    for (UInventoryComponent* Inventory : InInventories)
    {
        CopiedInventories.Add(Inventory->GetCopiedSlots());
    }

    for (const FIngredient& Ingredient : Recipe->Ingredients)
    {
        const UItemDataAsset* ItemData = Ingredient.ItemData;
        int32 RemainingCount = Ingredient.Quantity;

        if (!ItemData)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : %s 제작법의 재료에 nullptr 데이터가 존재합니다."),
                   *InRecipeId.ToString());
            return false;
        }

        // 인벤토리에서 뺄 수 있는 만큼 빼야한다
        // 앞 인벤토리부터 차례대로
        auto InventoryIter = CopiedInventories.begin();

        // 같은 종류의 아이템이 있는 슬롯을 찾아 모두 비우기
        int32 StartIndex = 0;
        while (RemainingCount > 0)
        {
            if (InventoryIter == CopiedInventories.end())
            {
                UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : 재고 검사와 실제 차감이 불일치함 - Ingredient: %s"),
                       ItemData ? *ItemData->GetName() : TEXT("nullptr"));
                return false;
            }

            int32 FoundIndex = FindFewestSlot__(*InventoryIter, ItemData, StartIndex);

            // 같은 종류의 아이템이 들어있는 슬롯을 찾았다
            if (FoundIndex >= 0)
            {
                int32 FoundSlotItemCount = (*InventoryIter)[FoundIndex].GetCount();
                int32 AmountToSubtract = FMath::Min(RemainingCount, FoundSlotItemCount);

                RemainingCount -= AmountToSubtract;
                (*InventoryIter)[FoundIndex].SetCount(FoundSlotItemCount - AmountToSubtract);
                StartIndex = FoundIndex + 1;
            }
            // 같은 종류의 아이템이 들어있는 슬롯이 없으면 다음 인벤토리로
            else
            {
                ++InventoryIter;
                StartIndex = 0;
            }
        }
    }

    // 재료 아이템 제거 후 남은 빈 인벤토리 슬롯 개수 측정
    for (const FIngredient& Result : Recipe->Results)
    {
        const UItemDataAsset* ItemData = Result.ItemData;
        int32 RemainingCount = Result.Quantity;

        if (!ItemData)
        {
            UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : %s 제작법의 결과물에 nullptr 데이터가 존재합니다."),
                   *InRecipeId.ToString());
            return false;
        }

        // 인벤토리에 더할 수 있는 만큼 더해야한다
        // 앞 인벤토리부터 차례대로
        auto InventoryIter = CopiedInventories.begin();

        // 같은 종류의 아이템이 있는 슬롯을 찾아 모두 더하기
        int32 StartIndex = 0;
        while (RemainingCount > 0)
        {
            if (InventoryIter == CopiedInventories.end())
            {
                UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::HasEnoughEmptySlots()] : %s 제작 결과물들을 담을 공간이 부족합니다."),
                       ItemData ? *ItemData->GetName() : TEXT("nullptr"));
                return false;
            }

            int32 FoundIndex = FindSlotWithItem__(*InventoryIter, ItemData, StartIndex);

            // 같은 종류의 아이템이 들어있는 슬롯을 찾았다
            if (FoundIndex >= 0)
            {
                int32 FoundSlotCount = (*InventoryIter)[FoundIndex].GetCount();
                int32 FoundSlotRemainingCount = (*InventoryIter)[FoundIndex].GetRemainingCount();
                int32 AmountToAdd = FMath::Min(RemainingCount, FoundSlotRemainingCount);

                RemainingCount -= AmountToAdd;
                (*InventoryIter)[FoundIndex].SetCount(FoundSlotCount + AmountToAdd);
                StartIndex = FoundIndex + 1;
            }
            // 같은 종류의 아이템이 들어있는 슬롯이 없으면 빈 슬롯에
            else
            {
                while (RemainingCount > 0)
                {
                    int32 EmptyIndex = FindEmptySlot__(*InventoryIter);

                    // 빈슬롯이 없으면 다음 인벤토리로
                    if (EmptyIndex < 0)
                    {
                        ++InventoryIter;
                        StartIndex = 0;
                        break;
                    }

                    int32 AmountToAdd = FMath::Min(ItemData->MaxStackCount, RemainingCount);

                    (*InventoryIter)[EmptyIndex].ItemData = ItemData;
                    (*InventoryIter)[EmptyIndex].SetCount(AmountToAdd);
                    RemainingCount -= AmountToAdd;
                }
            }
        }
    }

    return true;
}

bool UCraftingComponent::Craft(FName InRecipeId, const TArray<UInventoryComponent*>& InInventories)
{
    const FRecipeTableRow* Recipe = RecipeTable_->FindRow<FRecipeTableRow>(InRecipeId, TEXT("UCraftingComponent::Craft()"));

    if (!Recipe)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::Craft()] : 존재하지 않는 제작법입니다 (%s)"),
               *InRecipeId.ToString());
        return false;
    }

    if (!IsUnlockedRecipe(InRecipeId, Recipe->bLockedByDefault))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UCraftingComponent::Craft()] : %s 제작법은 아직 해금되지 않았습니다."),
               *InRecipeId.ToString());
        return false;
    }

    if (!HasEnoughIngredients(InRecipeId, InInventories))
    {
        return false;
    }

    if (!HasEnoughEmptySlots(InRecipeId, InInventories))
    {
        return false;
    }

    /* 실제로 재료 아이템을 소모하고 결과 아이템을 추가 */
    /* 이전 코드에서 제작 가능 여부를 전부 체크했으므로 제작에 반드시 성공한다고 가정함 */

    FInventoryCommandResult Result;

    // 재료 아이템 소모
    for (const FIngredient& Ingredient : Recipe->Ingredients)
    {
        const UItemDataAsset* ItemData = Ingredient.ItemData;
        int32 RemainingCount = Ingredient.Quantity;

        auto InventoryIter = InInventories.begin();

        while (RemainingCount > 0)
        {
            (*InventoryIter)->ExecuteCommand(
                FInventoryCommand::MakeSubtractCommand(ItemData, RemainingCount),
                Result
            );

            RemainingCount = Result.RemainingCount;

            if (RemainingCount > 0)
            {
                ++InventoryIter;
            }
        }
    }

    // 결과 아이템 추가
    for (const FIngredient& RecipeResult : Recipe->Results)
    {
        const UItemDataAsset* ItemData = RecipeResult.ItemData;
        int32 RemainingCount = RecipeResult.Quantity;

        auto InventoryIter = InInventories.begin();

        while (RemainingCount > 0)
        {
            (*InventoryIter)->ExecuteCommand(
                FInventoryCommand::MakeAddCommand(ItemData, RemainingCount),
                Result
            );

            RemainingCount = Result.RemainingCount;

            if (RemainingCount > 0)
            {
                ++InventoryIter;
            }
        }
    }

    return true;
}

void UCraftingComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UCraftingComponent::FindFewestSlot__(const TArray<FInventorySlot>& InSlots, const UItemDataAsset* InItemData, int32 StartIndex) const
{
    int32 FoundIndex = -1;
    int32 MinStackCount = TNumericLimits<int32>::Max();

    int32 SlotSize = InSlots.Num();
    for (int i = StartIndex; i < SlotSize; i++)
    {
        if (InSlots[i].ItemData == InItemData
            && InSlots[i].GetCount() < MinStackCount)
        {
            FoundIndex = i;
            MinStackCount = InSlots[i].GetCount();
        }
    }

    return FoundIndex;
}

int32 UCraftingComponent::FindSlotWithItem__(const TArray<FInventorySlot>& InSlots, const UItemDataAsset* InItemData, int32 StartIndex) const
{
    int32 SlotSize = InSlots.Num();
    for (int i = StartIndex; i < SlotSize; i++)
    {
        if (InSlots[i].ItemData == InItemData
            && !InSlots[i].IsFull())
        {
            return i;
        }
    }

    return -1;
}

// 빈 슬롯의 인덱스를 반환하는 함수
int32 UCraftingComponent::FindEmptySlot__(const TArray<FInventorySlot>& InSlots) const
{
    int32 SlotSize = InSlots.Num();
    for (int i = 0; i < SlotSize; i++)
    {
        if (InSlots[i].IsEmpty())
        {
            return i;
        }
    }

    return -1;
}
