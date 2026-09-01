// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ItemAction/ItemAction.h"
#include "Data/Item/EquipmentDataAsset.h"
#include "EquipItemAction.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UEquipItemAction : public UItemAction
{
	GENERATED_BODY()
	
public:
	// 에디터/인벤토리에서 지정할 장비 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TObjectPtr<UEquipmentDataAsset> EquipmentData;

	// 아이템 사용/장착 실행 함수 오버라이드
	virtual void ExecuteItemAction_Implementation(AActor* InInstigator, AActor* InTarget) override;
};
