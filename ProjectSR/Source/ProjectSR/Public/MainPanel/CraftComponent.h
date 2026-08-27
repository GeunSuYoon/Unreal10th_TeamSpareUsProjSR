// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnItemCraftDataAssetChange);

class UInventoryComponent;
class UItemCraftDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSR_API UCraftComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// InCraftBlueprint의 재료 아이템을 InSourceInventory에서 탐색, 이후 존재하면 결과 아이템을 OutTargetInventory에 추가하는 함수.
	void	CraftItem(UItemCraftDataAsset* InItemCraftDataAsset, UInventoryComponent* InSourceInventory, UInventoryComponent* OutTargetInventory);

	//void	

	FOnItemCraftDataAssetChange	OnItemCraftDataAssetChange;

protected:
	//UPROPERTY(Blue)
	// 현재 가지고 있는 아이템 재작 설계도 목록
	TArray<UItemCraftDataAsset*>	ItemCraftDataAssetList_;

};
