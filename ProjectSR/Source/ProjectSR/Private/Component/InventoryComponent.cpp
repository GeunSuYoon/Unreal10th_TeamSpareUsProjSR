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
            HandleAddCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Subtract:
            HandleSubtractCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Search:
            HandleSearchCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Move:
            HandleMoveCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Drop:
            HandleDropCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Use:
            HandleUseCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Clear:
            HandleClearCommand_(Command, OutResult);
            break;
        case EInventoryCommandType::Equip:
            HandleEquipCommand_(Command, OutResult);
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
    if (!IsValidIndex(InSlotIndex)) return nullptr;
    //check(IsValidIndex(InSlotIndex));

    // check(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 안됨
    // verify(bool)	: bool이 거짓이면 프로그램 종료. shipping 빌드에 포함이 됨
    // ensure(bool) : bool이 거짓이면 로그 출력 후 계속. shipping 빌드에 포함이 됨

    return &Slots_[InSlotIndex];
}

FInventorySlot* UInventoryComponent::GetTempSlot()
{
    return &Slots_[InventorySize];	// 무조건 마지막 슬롯이 Temp슬롯
}

bool UInventoryComponent::HandleAddCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    int32 RemainingCount = AddItem_(Command.ItemData, Command.Count);

    //RemainingCount가 0이면 인벤토리에 잘 들어갔음. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
    OutResult.bSuccess = true;
    OutResult.RemainingCount = RemainingCount;

    if (OutResult.RemainingCount > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("[UInventoryComponent::HandleAddCommand_()] : %s 추가가 성공적으로 완료되었습니다."), *(Command.ItemData->DisplayName.ToString()));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[UInventoryComponent::HandleAddCommand_()] : %d개의 아이템이 남았습니다."), OutResult.RemainingCount);
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleSubtractCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!Command.ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleSubtractCommand_()] : Command.ItemData가 nullptr 입니다."));
        return OutResult.bSuccess;
    }

    if (Command.Count <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleSubtractCommand_()] : Command.Count가 0 이하입니다."));
        return OutResult.bSuccess;
    }

    int32 RemainingCount = SubtractItem_(Command.ItemData, Command.Count);
    OutResult.bSuccess = true;
    OutResult.RemainingCount = RemainingCount;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleSearchCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!Command.ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleSearchCommand_()] : Command.ItemData가 nullptr 입니다."));
        return OutResult.bSuccess;
    }

    if (Command.Count <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleSearchCommand_()] : Command.Count가 0 이하입니다."));
        return OutResult.bSuccess;
    }

    OutResult.bSuccess = GetTotalItemCount(Command.ItemData) >= Command.Count;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleMoveCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(Command.SourceIndex) || !IsValidIndex(Command.TargetIndex))
    {
        return OutResult.bSuccess;
    }

    FInventorySlot* SourceSlot = Command.SourceInventoryComponent->GetSlot(Command.SourceIndex);
    FInventorySlot* TargetSlot = GetSlot(Command.TargetIndex);

    // 소스가 비어있으면 실패(처리안함)
    if (SourceSlot->IsEmpty())
    {
        return OutResult.bSuccess;
    }

    if (TargetSlot->IsEmpty())
    {
        // 대상 슬롯이 비어있다 => 그대로 이동처리
        SetSlot(Command.TargetIndex, SourceSlot->ItemData, SourceSlot->GetCount());
        Command.SourceInventoryComponent->ClearSlot(Command.SourceIndex);

        OutResult.bSuccess = true;
    }
    else if (TargetSlot->ItemData == SourceSlot->ItemData)
    {
        // 이동을 시키는데 같은 아이템이 들어있다. => 병합 처리
        int32 AmountToAdd = FMath::Min(TargetSlot->GetRemainingCount(), SourceSlot->GetCount());
        if (AmountToAdd > 0)
        {
            UpdateSlotCount(Command.TargetIndex, AmountToAdd);
            Command.SourceInventoryComponent->UpdateSlotCount(Command.SourceIndex, -AmountToAdd);

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
        const UItemDataAsset* SourceItem = SourceSlot->ItemData;
        const UItemDataAsset* TargetItem = TargetSlot->ItemData;
        int32 SourceCount = SourceSlot->GetCount();
        int32 TargetCount = TargetSlot->GetCount();

        Command.SourceInventoryComponent->SetSlot(Command.SourceIndex, TargetItem, TargetCount);
        SetSlot(Command.TargetIndex, SourceItem, SourceCount);

        OutResult.bSuccess = true;
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleDropCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    FInventorySlot* Slot = GetSlot(Command.TargetIndex);

    if (Slot->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::HandleDropCommand_()] : %d번 슬롯이 비어 있습니다."),
               Command.TargetIndex);
        return OutResult.bSuccess;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[UInventoryComponent::HandleDropCommand_()] : World가 nullptr입니다!"));
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
        SpawnLoction += Command.DropLocation;

        FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoction);
        Factory->SpawnItemActorAsync(Slot->ItemData, SpawnTransform, FOnPickupSpawned());
    }

    ClearSlot(Command.TargetIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleUseCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex(Command.TargetIndex))
    {
        return OutResult.bSuccess;
    }

    UseItem_(Command.TargetIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleClearCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex(Command.TargetIndex))
    {
        return OutResult.bSuccess;
    }

    ClearSlot(Command.TargetIndex);
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
//    ClearSlot(InSlotIndex);
//
//    OutResult.bSuccess = true;
//    return OutResult.bSuccess;
//}

bool UInventoryComponent::HandleEquipCommand_(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;
    if (!IsValidIndex(Command.TargetIndex))
    {
        return OutResult.bSuccess;
    }

    EquipItem_(Command.TargetIndex);
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
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::AddItem_()] : InItemData가 nullptr 입니다."));
        return InCount;
    }

    if (InCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::AddItem_()] : InCount가 0이하 입니다."));
        return InCount;
    }

    int32 RemainingCount = InCount;
    int32 StartIndex = 0;

    // 같은 종류의 아이템이 있는 슬롯을 찾아 최대한 채우기
    while (RemainingCount > 0)	// 남는게 있으면 계속 반복
    {
        // 같은 종류의 아이템이 들어있는 슬롯을 찾아 추가하기
        int32 FoundIndex = FindSlotWithItem__(InItemData, true, StartIndex);

        // 같은 종류의 아이템이 들어있는 슬롯이 없으면 종료
        if (FoundIndex == InventoryFail)
        {
            break;
        }

        // 같은 종류의 아이템이 들어있는 슬롯을 찾았다.
        FInventorySlot* Slot = GetSlot(FoundIndex);
        int32 AmountToAdd = FMath::Min(Slot->GetRemainingCount(), RemainingCount);
        UpdateSlotCount(FoundIndex, AmountToAdd);	// FoundIndex 슬롯에 채울 수 있는 만큼 채우기

        RemainingCount -= AmountToAdd;	// 남은 개수 갱신
        StartIndex = FoundIndex + 1;	// 새 시작 위치 갱신
    }

    // 빈슬롯을 찾아 최대한 채우기
    while (RemainingCount > 0)
    {
        int32 EmptyIndex = FindEmptySlot__();

        // 빈슬롯이 없으면 종료
        if (EmptyIndex == InventoryFail)
        {
            break;
        }

        int32 AmountToAdd = FMath::Min(InItemData->MaxStackCount, RemainingCount);
        SetSlot(EmptyIndex, InItemData, AmountToAdd);	// EmptyIndex 슬롯에 아이템 설정

        RemainingCount -= AmountToAdd;	// 남은 개수 갱신
    }

    // RemainingCount가 0이면 인벤토리에 잘 들어감. 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
    return RemainingCount;
}

int32 UInventoryComponent::SubtractItem_(const UItemDataAsset* InItemData, int32 InCount)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::SubtractItem_] : InItemData가 nullptr 입니다."));
        return InCount;
    }

    if (InCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::SubtractItem_] : InCount가 0이하 입니다."));
        return InCount;
    }

    int32 RemainingCount = InCount;
    int32 StartIndex = 0;

    // 같은 종류의 아이템이 있는 슬롯을 찾아 모두 비우기
    while (RemainingCount > 0) // 남는게 있으면 계속 반복
    {
        // 같은 종류의 아이템이 들어있는 슬롯을 찾아 비우기
        int32 FoundIndex = FindSlotWithItem__(InItemData, false, StartIndex);

        // 같은 종류의 아이템이 들어있는 슬롯이 없으면 종료
        if (FoundIndex == InventoryFail)
        {
            break;
        }

        // 같은 종류의 아이템이 들어있는 슬롯을 찾았다
        FInventorySlot* Slot = GetSlot(FoundIndex);
        int32 AmountToSubtract = FMath::Min(Slot->GetCount(), RemainingCount);
        UpdateSlotCount(FoundIndex, -AmountToSubtract); // FoundIndex 슬롯을 비울 수 있는 만큼 비우기

        RemainingCount -= AmountToSubtract;	// 남은 개수 갱신
        StartIndex = FoundIndex + 1;	// 새 시작 위치 갱신
    }

    return RemainingCount;
}

int32 UInventoryComponent::GetTotalItemCount(const UItemDataAsset* InItemData)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::GetTotalItemCount()] : InItemData가 nullptr 이므로 0을 반환합니다."));
        return 0;
    }

    int32 TotalCount = 0;

    for (int32 i = 0; i < InventorySize; i++)
    {
        if (Slots_[i].ItemData == InItemData)
        {
            TotalCount += Slots_[i].GetCount();
        }
    }

    return TotalCount;
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
    UpdateSlotCount(InIndex, -1);
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
    //            UpdateSlotCount(InIndex, -1);
    //        }
    //    }
    //}
}

void UInventoryComponent::SetSlot(int32 InSlotIndex, const UItemDataAsset* InItemData, int32 InCount)
{
    FInventorySlot* Slot = GetSlot(InSlotIndex);

    if (!Slot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::SetSlot()] : %d번 슬롯 불러오기 오류."),
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

void UInventoryComponent::UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount)
{
    FInventorySlot* Slot = GetSlot(InSlotIndex);

    if (Slot->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[UInventoryComponent::UpdateSlotCount()] : Slot이 비어 있습니다."));
        return;
    }

    int32 NewCount = Slot->GetCount() + InDeltaCount;

    SetSlot(InSlotIndex, Slot->ItemData, NewCount);
}

void UInventoryComponent::ClearSlot(int32 InSlotIndex)
{
    SetSlot(InSlotIndex, nullptr, 0);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UInventoryComponent::FindSlotWithItem__(const UItemDataAsset* InItemData, bool bCheckFull, int32 InStartIndex)
{
    int32 Result = InventoryFail;

    for (int32 i = InStartIndex; i < InventorySize; i++)
    {
        if (!Slots_[i].bDragging
            && Slots_[i].ItemData == InItemData
            && (!bCheckFull || !Slots_[i].IsFull()))
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
        if (!Slots_[i].bDragging
            && Slots_[i].IsEmpty())
        {
            Result = i;
            break;
        }
    }

    return Result;
}
