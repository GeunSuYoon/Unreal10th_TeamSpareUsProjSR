// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"
#include "Data/ItemAction/ItemAction.h"
#include "Framework/SubSystem/ItemActorFactorySubsystem.h"
//#include "Data/Item/UseableItemDataAsset.h"
//#include "Data/Item/WeaponDataAsset.h"
//#include "Interface/WeaponUserInterface.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    Slots_.SetNum(InventorySize + 1);	// 일반 슬롯 10개 + 임시 슬롯 1개
}

bool UInventoryComponent::ExecuteCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    switch (Command.Type)
    {
        case EInventoryCommandType::Add:
            HandleAddCommand_(Command.ItemData, Command.Count, OutResult);
            break;
        case EInventoryCommandType::Search:
            HandleSearchCommand_(Command.ItemData, Command.Count, OutResult);
            break;
        case EInventoryCommandType::Move:
            HandleMoveCommand_(Command.SourceIndex, Command.TargetIndex, OutResult);
            break;
        case EInventoryCommandType::Drop:
            HandleDropCommand_(Command.SourceIndex, Command.DropLocation, OutResult);
            break;
        case EInventoryCommandType::Use:
            HandleUseCommand_(Command.SourceIndex, OutResult);
            break;
        case EInventoryCommandType::Clear:
            HandleClearCommand_(Command.TargetIndex, OutResult);
            break;
        case EInventoryCommandType::Equip:
            HandleEquipCommand_(Command.SourceIndex, OutResult);
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("알 수 없는 커맨드 입니다"));
            break;
    }

    return OutResult.bSuccess;
}

void UInventoryComponent::ShowInventory()
{
    for (int i = 0; i < InventorySize; i++)
    {
        if (!Slots_[i].IsEmpty())
        {
            UE_LOG(LogTemp, Log, TEXT("[UInventoryComponent::ShowInventory()] : %d번 슬롯 -  %s %d개"),
                   i, *Slots_[i].ItemData->DisplayName.ToString(), Slots_[i].GetCount());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[UInventoryComponent::ShowInventory()] : %d번 슬롯 - 비어 있음"), i);
        }
    }
}

FInventorySlot* UInventoryComponent::GetSlot(int InSlotIndex)
{
    //if (!IsValidIndex_(InSlotIndex)) return nullptr;
    check(IsValidIndex_(InSlotIndex));

    // check(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 안됨
    // verify(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 됨
    // ensure(bool) : bool이 거짓이면 로그 출력 후 계속. shipping 빌드에 포함이 됨

    return &Slots_[InSlotIndex];
}

FInventorySlot* UInventoryComponent::GetTempSlot()
{
    return &Slots_[TempSlotIndex];	// 무조건 마지막 슬롯이 Temp슬롯
}

bool UInventoryComponent::HandleAddCommand_(const UItemDataAsset* InItemData, int32 InCount, FInventoryCommandResult& OutResult)
{
    int32 RemainingCount = AddItem_(InItemData, InCount);

    //RemainingCount가 0이면 인벤토리에 잘 들어갔음. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
    if (RemainingCount > 0)
    {
        OutResult.bSuccess = false;
        OutResult.RemainingCount = RemainingCount;
    }
    else
    {
        OutResult.bSuccess = true;
        OutResult.RemainingCount = 0;
    }

    if (OutResult.bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] 추가가 성공적으로 완료되었습니다."), *(InItemData->DisplayName.ToString()));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("%d개의 아이템이 남았습니다."), OutResult.RemainingCount);
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleSearchCommand_(const UItemDataAsset* InItemData, int32 InCount, FInventoryCommandResult& OutResult)
{
    return false;
}

bool UInventoryComponent::HandleMoveCommand_(int32 InSourceIndex, int32 InTargetIndex, FInventoryCommandResult& OutResult)
{
    if (!IsValidIndex_(InSourceIndex) || !IsValidIndex_(InTargetIndex))
    {
        OutResult.bSuccess = false;
        return false;
    }

    if (InSourceIndex == InTargetIndex)
    {
        OutResult.bSuccess = true;
        return true;
    }

    FInventorySlot& SourceSlot = Slots_[InSourceIndex];
    FInventorySlot& TargetSlot = Slots_[InTargetIndex];

    // 소스가 비어있으면 실패(처리안함)
    if (SourceSlot.IsEmpty())
    {
        OutResult.bSuccess = false;
        return false;
    }

    if (TargetSlot.IsEmpty())
    {
        // 대상 슬롯이 비어있다 => 그대로 이동처리
        SetSlot_(InTargetIndex, SourceSlot.ItemData, SourceSlot.GetCount());
        ClearSlot_(InSourceIndex);
        OutResult.bSuccess = true;
    }
    else if (TargetSlot.ItemData == SourceSlot.ItemData)
    {
        // 이동을 시키는데 같은 아이템이 들어있다. => 병합 처리
        int32 AmountToAdd = FMath::Min(TargetSlot.GetRemainingCount(), SourceSlot.GetCount());
        if (AmountToAdd > 0)
        {
            UpdateSlotCount_(InTargetIndex, AmountToAdd);
            UpdateSlotCount_(InSourceIndex, -AmountToAdd);
            OutResult.bSuccess = true;
        }
        else
        {
            OutResult.bSuccess = false;
        }
    }
    else
    {
        // 소스와 타겟이 서로 다른 아이템이다 => 슬롯 스왑
        const UItemDataAsset* SourceItem = SourceSlot.ItemData;
        int32 SourceCount = SourceSlot.GetCount();
        const UItemDataAsset* TargetItem = TargetSlot.ItemData;
        int32 TargetCount = TargetSlot.GetCount();

        SetSlot_(InSourceIndex, TargetItem, TargetCount);
        SetSlot_(InTargetIndex, SourceItem, SourceCount);
        OutResult.bSuccess = true;
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleDropCommand_(int32 InSlotIndex, const FVector& InDropLocation, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    FInventorySlot* Slot = GetSlot(InSlotIndex);

    if (Slot->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleDropCommand_()] : %d번 슬롯이 비어 있습니다."),
               InSlotIndex);
        return OutResult.bSuccess;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleDropCommand_()] : World nullptr."));
        return OutResult.bSuccess;
    }

    UItemActorFactorySubsystem* Factory = World->GetSubsystem<UItemActorFactorySubsystem>();

    if (!Factory)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleDropCommand_()] : ItemActorFactorySubsystem nullptr."));
        return OutResult.bSuccess;
    }

    for (int32 _ = 0; _ < Slot->GetCount(); _++)
    {
        FVector SpawnLoction(FMath::RandPointInCircle(100.0f), 0);
        SpawnLoction += InDropLocation;

        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoction);
        Factory->SpawnItemActorAsync(Slot->ItemData, SpawnTransform, FOnPickupSpawned());
    }

    ClearSlot_(InSlotIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleUseCommand_(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex_(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    UseItem_(InSlotIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleClearCommand_(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex_(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    ClearSlot_(InSlotIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

//bool UInventoryComponent::HandleMoneyCommand(int32 InMoneyDiff, FInventoryCommandResult& OutResult)
//{
//    OutResult.bSuccess = false;
//
//    AddMoney(InMoneyDiff);
//    OutResult.bSuccess = true;
//
//    return OutResult.bSuccess;
//}
//
//bool UInventoryComponent::HandleSellCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult)
//{
//    FInventorySlot* TargetSlot = GetSlot(InSlotIndex);
//    if (TargetSlot->IsEmpty())
//    {
//        OutResult.bSuccess = false;
//        return OutResult.bSuccess;
//    }
//
//    int32 SellPrice = TargetSlot->ItemData->Price * 0.5f;
//    AddMoney(SellPrice * TargetSlot->GetCount());
//
//    ClearSlot_(InSlotIndex);
//
//    OutResult.bSuccess = true;
//    return OutResult.bSuccess;
//}

bool UInventoryComponent::HandleEquipCommand_(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex_(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    EquipItem_(InSlotIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

//void UInventoryComponent::AddMoney(int32 InIncome)
//{
//    Money += InIncome;
//    OnMoneyChanged.Broadcast(Money);	// 돈의 변경을 알림
//}

int32 UInventoryComponent::AddItem_(const UItemDataAsset* InItemData, int32 InCount)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("InItemData가 null입니다."));
        return InCount;
    }
    if (InCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("InCount가 0이하 입니다."));
        return InCount;
    }

    int32 RemainingCount = InCount;

    int32 StartIndex = 0;
    // 같은 종류의 아이템이 있는 슬롯을 찾아 최대한 채우기
    while (RemainingCount > 0)	// 남는게 있으면 계속 반복
    {
        // 같은 종류의 아이템이 들어있는 슬롯을 찾아 추가하기
        int32 FoundIndex = FindSlotWithItem__(InItemData, StartIndex);
        if (FoundIndex == InventoryFail) break;	// 같은 종류의 아이템이 들어있는 슬롯이 없으면 종료

        // 같은 종류의 아이템이 들어있는 슬롯을 찾았다.
        FInventorySlot& Slot = Slots_[FoundIndex];
        int32 AmountToAdd = FMath::Min(Slot.GetRemainingCount(), RemainingCount);
        UpdateSlotCount_(FoundIndex, AmountToAdd);	// FoundIndex 슬롯에 채울 수 있는 만큼 채우기

        RemainingCount -= AmountToAdd;	// 남은 개수 갱신
        StartIndex = FoundIndex + 1;	// 새 시작 위치 갱신
    }

    // 빈슬롯을 찾아 최대한 채우기
    while (RemainingCount > 0)
    {
        int32 EmptyIndex = FindEmptySlot__();
        if (EmptyIndex == InventoryFail) break;	// 빈슬롯이 없으면 종료

        FInventorySlot& Slot = Slots_[EmptyIndex];
        int32 AmountToAdd = FMath::Min(InItemData->MaxStackCount, RemainingCount);
        SetSlot_(EmptyIndex, InItemData, AmountToAdd);	// EmptyIndex 슬롯에 아이템 설정

        RemainingCount -= AmountToAdd;	// 남은 개수 갱신
    }

    // RemainingCount가 0이면 인벤토리에 잘 들어감. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
    return RemainingCount;
}

void UInventoryComponent::UseItem_(int32 InIndex)
{
    FInventorySlot* Slot = GetSlot(InIndex);

    if (!Slot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::UseItem_()] : Slot 이 nullptr 입니다."));
        return;
    }

    if (Slot->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::UseItem_()] : Slot 이 비어있습니다."));
        return;
    }

    if (!Slot->ItemData->ItemAction)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::UseItem_()] : Slot 아이템은 사용할 수 없습니다."));
        return;
    }

    Slot->ItemData->ItemAction->ExecuteItemAction_Implementation(GetOwner(), GetOwner());
    UpdateSlotCount_(InIndex, -1);
}

void UInventoryComponent::EquipItem_(int32 InIndex)
{
    UE_LOG(LogTemp, Log, TEXT("[UInventoryComponent::EquipItem_()] : EquipItem_."));
    //if (FInventorySlot* InvenSlot = GetSlot(InIndex))
    //{
    //    if (const UWeaponDataAsset* Weapon = Cast<const UWeaponDataAsset>(InvenSlot->ItemData))
    //    {
    //        if (GetOwner()->Implements<UWeaponUserInterface>())
    //        {
    //            IWeaponUserInterface::Execute_EquipWeapon(GetOwner(), Weapon);
    //            UpdateSlotCount_(InIndex, -1);
    //        }
    //    }
    //}
}

void UInventoryComponent::SetSlot_(int32 InSlotIndex, const UItemDataAsset* InItemData, int32 InCount)
{
    FInventorySlot* Slot = GetSlot(InSlotIndex);

    if (!Slot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::SetSlot_()] : %d번 슬롯 불러오기 오류."),
               InSlotIndex);
        return;
    }

    Slot->ItemData = InItemData;
    Slot->SetCount(InCount);

    if (InItemData && !InItemData->IsLoaded())
    {
        InItemData->RequestDataLoad(
            FStreamableDelegate::CreateWeakLambda(
                this,
                [this, InSlotIndex]() {
                    // 리프레시용으로 변경 브로드 캐스트 날리기
                    OnSlotChanged.ExecuteIfBound(InSlotIndex);
                })
        );
    }

    // 델리게이트 호출
    OnSlotChanged.ExecuteIfBound(InSlotIndex);
}

void UInventoryComponent::UpdateSlotCount_(int32 InSlotIndex, int32 InDeltaCount)
{
    if (!IsValidIndex_(InSlotIndex)) return;

    FInventorySlot& Slot = Slots_[InSlotIndex];
    if (Slot.IsEmpty()) return;

    int32 NewCount = Slot.GetCount() + InDeltaCount;
    SetSlot_(InSlotIndex, Slot.ItemData, NewCount);
}

void UInventoryComponent::ClearSlot_(int32 InSlotIndex)
{
    SetSlot_(InSlotIndex, nullptr, 0);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UInventoryComponent::FindSlotWithItem__(const UItemDataAsset* InItemData, int32 InStartIndex)
{
    int32 Result = InventoryFail;

    for (int32 i = InStartIndex; i < InventorySize; i++)
    {
        if (Slots_[i].ItemData == InItemData && !Slots_[i].IsFull())	// 같은 종류의 아이템인데 스택이 남아있는 경우
        {
            Result = i;
            break;
        }
    }

    return Result;
}

int32 UInventoryComponent::FindEmptySlot__()
{
    int32 Result = InventoryFail;
    for (int32 i = 0; i < InventorySize; i++)
    {
        if (Slots_[i].IsEmpty())
        {
            Result = i;
            break;
        }
    }

    return Result;
}
