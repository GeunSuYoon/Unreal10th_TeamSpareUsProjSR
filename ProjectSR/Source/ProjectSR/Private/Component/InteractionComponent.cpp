// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InteractionComponent.h"
#include "Interface/InteractInterface.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// 메인 상호작용 함수
void UInteractionComponent::PlayerInteract()
{
	AActor* TargetActor = FindInteractableObject();
	if (!TargetActor) return;

	// 대상이 IInteractInterface를 구현했는지 확인 후 인터페이스 함수 호출
	if (TargetActor->GetClass()->ImplementsInterface(UInteractInterface::StaticClass()))
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		IInteractInterface::Execute_Interact(TargetActor, OwnerPawn);
	}
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// 라인트레이스로 인터페이스를 가지고 있는 대상 탐지
AActor* UInteractionComponent::FindInteractableObject()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return nullptr;

	// 카메라의 위치와 시선 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	float	CameraToPlayerDistance = FVector::Distance(CameraLocation, OwnerPawn->GetActorLocation());
	float	TraceDistance = CameraToPlayerDistance + InteractionDistance;
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * TraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerPawn); // 자기 자신(플레이어)은 감지 대상에서 제외

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TraceChannel,
		QueryParams
	);
	
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		bHit ? FColor::Green : FColor::Red,
		false,
		2.0f,
		0,
		2.0f
	);

	if (bHit && HitResult.GetActor())
	{
		return HitResult.GetActor();
	}

	return nullptr;
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

