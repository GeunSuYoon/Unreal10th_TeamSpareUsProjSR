// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipComponent.generated.h"

// 장비 슬롯 구조체
UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None		UMETA(DisplayName = "None"),
	SpaceSuit	UMETA(DisplayName = "SpaceSuit"),
	DragMachine	UMETA(DisplayName = "DragMachine"),
	Weapon		UMETA(DisplayName = "Weapon")
};

// 장비 교체 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChangedSignature, EEquipmentSlot, Slot, AActor*, EquippedSlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipComponent();

	// 슬롯별 장비 장착-해제 함수
	UFUNCTION(BlueprintCallable, Category = "Equip")
	bool EquipItem(EEquipmentSlot Slot, TSubclassOf<AActor> EquipActorClass);

	UFUNCTION(BlueprintCallable, Category = "Equip")
	bool UnEquipItem(EEquipmentSlot Slot);
	
	// 현재 장착된 액터 가져오기
	UFUNCTION(BlueprintCallable, Category = "Equip")
	AActor* GetEquippedActor(EEquipmentSlot Slot) const;

public:
	// 스탯, 인벤토리 컴포넌트나 UI가 구독할 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Equip|Events")
	FOnEquipmentChangedSignature OnEquipmentChanged;

protected:
	// 슬롯별 현재 생성되어 장착된 액터들을 관리하는 맵
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equip")
	TMap<EEquipmentSlot, AActor*> EquippedActors;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
