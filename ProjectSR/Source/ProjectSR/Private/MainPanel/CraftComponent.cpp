// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPanel/CraftComponent.h"

// Sets default values for this component's properties
UCraftComponent::UCraftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCraftComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCraftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// InSourceInventory에 InCraftBlueprint의 재료 아이템 탐색 성공 시 재료 아이템을 지우고 OutTargetInventory에 최종 아이템을 반환
void UCraftComponent::CraftItem(UItemCraftDataAsset* InItemCraftDataAsset, UInventoryComponent* InSourceInventory, UInventoryComponent* OutTargetInventory)
{
	// SourceInventory에 재료 아이템 찾는 코드
	// 재료 아이템 있으면 TargetInventory에 빈 자리 있는지 찾는 코드
	// 빈 자리까지 있으면 SourceInventory에서 재료 아이템 삭제
	// TargetInventory에 목표 아이템 추가
}

