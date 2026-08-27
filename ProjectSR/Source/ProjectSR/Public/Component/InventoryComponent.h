// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/InventoryCommandTypes.h"
#include "Data/Item/ItemDataAsset.h"

#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

//class UTemporarySlotWidget;

DECLARE_DELEGATE_OneParam(FOnInventorySlotChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryMoneyChanged, int32);

USTRUCT(BlueprintType)
struct FInvenSlot
{
    GENERATED_BODY()

public:
    // 이 슬롯에 들어있는 아이템의 종류
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    TObjectPtr<const UItemDataAsset> ItemData;

protected:
    // 이 슬롯에 들어있는 아이템의 개수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    int32 Count = 0;

public:
    // 슬롯이 비었는지 확인하는 함수
    inline bool IsEmpty() const { return ItemData == nullptr; }

    // 슬롯이 가득차있는지 확인하는 함수
    inline bool IsFull() const { return ItemData && (Count >= ItemData->MaxStackCount); }

    // 슬롯을 비우는 함수
    inline void Clear()
    {
        ItemData = nullptr;
        Count = 0;
    }

    // 현재 슬롯에 남아있는 스택 여유분 확인 함수
    inline int32 GetRemainingCount() const { return ItemData ? ItemData->MaxStackCount - Count : 0; }

    // 슬롯에 들었는 아이템 수를 확인하는 함수
    inline int32 GetCount() const { return Count; }

    // 슬롯에 아이템 수를 설정하는 함수
    inline void SetCount(int32 InCount)
    {
        if (ItemData && InCount > 0)
        {
            Count = FMath::Clamp(InCount, 0, ItemData->MaxStackCount);
        }
        else
        {
            Clear();
        }
    }

};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UInventoryComponent();

    // 커맨드 실행용 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory|Command")
    bool ExecuteCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult);

    // Getter ------------------------------------------------------------
    // 현재 돈을 리턴하는 함수
    inline int32 GetMoney() const { return Money; }

    // 특정 슬롯을 리턴하는 함수
    FInvenSlot* GetSlot(int InSlotIndex);

    // 임시 슬롯을 리턴하는 함수
    FInvenSlot* GetTempSlot();

    inline int32 GetTempSlotIndex() const { return TempSlotIndex; }

    // 인벤토리 크기를 리턴하는 함수
    inline int32 GetSize() const { return InventorySize; }

    // 임시 슬롯의 위젯 클래스를 리턴하는 함수
    //inline TSubclassOf<UTemporarySlotWidget> GetTemporasySlotWidgetClass() const { return TemporarySlotWidgetClass; }
    // --------------------------------------------------------------------

protected:
    // 인벤토리에 돈을 추가하거나 감소시키는 함수
    UFUNCTION(BlueprintCallable)
    void AddMoney(int32 InIncome);

    // 인벤토리에 아이템을 추가하는 함수
    UFUNCTION(BlueprintCallable)
    int32 AddItem(const UItemDataAsset* InItemData, int32 InCount);

    // 인벤토리의 특정 슬롯에 들어있는 아이템 사용하는 함수
    void UseItem(int32 InIndex);

    // 인벤토리의 특정 슬롯에 들어있는 아이템을 장비하는 함수
    void EquipItem(int32 InIndex);

    // 특정 슬롯에 아이템과 개수를 설정하는 함수
    void SetSlot(int32 InSlotIndex, const UItemDataAsset* InItemData, int32 InCount);

    // 특정 슬롯의 아이템 개수를 업데이트 하는 함수
    void UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount);

    // 특정 슬롯을 비우는 함수
    void ClearSlot(int32 InSlotIndex);

    // 인덱스가 적절한 범위인지 확인하는 함수
    inline bool IsValidIndex(int32 InSlotIndex) const {
        return (InSlotIndex <= InventorySize) && (InSlotIndex >= 0);
    }; // 임시슬롯 때문에 접근 범위는 InventorySize까지

    // 커맨드 핸들링 함수들 ----------------------------------------------------------------------------------------
    bool HandleAddCommand(const UItemDataAsset* InItemData, int32 InCount, FInventoryCommandResult& OutResult);
    bool HandleMoveCommand(int32 InSourceIndex, int32 InTargetIndex, FInventoryCommandResult& OutResult);
    bool HandleDropCommand(int32 InSlotIndex, const FVector& InDropLocation, FInventoryCommandResult& OutResult);
    bool HandleUseCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    bool HandleClearCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    //bool HandleMoneyCommand(int32 InMoneyDiff, FInventoryCommandResult& OutResult);
    //bool HandleSellCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    bool HandleEquipCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    // ------------------------------------------------------------------------------------------------------------

    // 사용안함. Called when the game starts
    virtual void BeginPlay() override;

    // 사용안함. Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // 같은 종류의 아이템이 있는 슬롯을 찾는 함수(남은 스택이 있어야함)
    int32 FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex = 0);

    // 비어있는 슬롯을 찾는 함수
    int32 FindEmptySlot();

public:
    // 슬롯에 변화가 생겼을 때 발동할 델리게이트(싱글캐스트)
    FOnInventorySlotChanged OnSlotChanged;

    // 돈에 변화가 생겼을 때 발동할 델리게이트(멀티캐스트)
    FOnInventoryMoneyChanged OnMoneyChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Money")
    int32 Money = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
    TArray<FInvenSlot> Slots;	// 크기는 InventorySize + 1(임시 슬롯)

    //protected:
    //    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
    //    TSubclassOf<UTemporarySlotWidget> TemporarySlotWidgetClass;

private:
    // 인벤토리의 크기
    static constexpr int32 InventorySize = 10;

    // 임시 슬롯의 인덱스
    static constexpr int32 TempSlotIndex = InventorySize;

    // 인벤토리 컴포넌트 함수에서 각종 실패 표시용 정수
    static constexpr int32 InventoryFail = -1;

};
