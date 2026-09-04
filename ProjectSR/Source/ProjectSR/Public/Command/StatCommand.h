// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatCommand.generated.h"

UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	Health,
	Hunger,
	Oxygen
};

USTRUCT(BlueprintType)
struct FStatChangeCommand
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerStatType StatType = EPlayerStatType::Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.0f;

	// 이력 추적용 (누가 왜 이 변경을 요청했는지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Source;
};