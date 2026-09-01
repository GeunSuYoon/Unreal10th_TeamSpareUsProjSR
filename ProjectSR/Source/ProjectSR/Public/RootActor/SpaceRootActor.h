// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceRootActor.generated.h"

// 테스트용 나중에 지워야함
class UMeteorAvoidanceComponent;

UCLASS()
class PROJECTSR_API ASpaceRootActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpaceRootActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//void	RotateSpaceRoot(const FVector2D& InInput, const float InRotateSpeed);
	UFUNCTION(BlueprintCallable)
	void	RotateSpaceRoot(const FRotator& InRotate);

	UFUNCTION(BlueprintCallable)
	void	InputMove(const FVector2D& InMove);

	inline USceneComponent* GetBackgroundPivot() const { return (this->BackgroundPivot_); };
	inline USceneComponent* GetItemPivot() const { return (this->ItemPivot_); };
	inline USceneComponent* GetMeteorPivot() const { return (this->MeteorPivot_); };
	//USceneComponent* GetBackgroundPivot() const;
	//USceneComponent* GetBackgroundPivot() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TestComponent")
	TObjectPtr<UMeteorAvoidanceComponent>	TestMeteorAvoidanceComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Space")
	TObjectPtr<USceneComponent> RootScene_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Space")
	TObjectPtr<USceneComponent> BackgroundPivot_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Space")
	TObjectPtr<USceneComponent> ItemPivot_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Space")
	TObjectPtr<USceneComponent> SpawnShellPivot_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Space")
	TObjectPtr<USceneComponent> MeteorPivot_;

};
