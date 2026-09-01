// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Data/SpaceMap/SpaceMapDataAsset.h"
#include "SpaceShip/SpaceShipActor.h"

// Sets default values for this component's properties
UMeteorAvoidanceComponent::UMeteorAvoidanceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMeteorAvoidanceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UMeteorAvoidanceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 몇 분 후 운석 충돌 관측 시 불려올 함수
void UMeteorAvoidanceComponent::MeteorDetect(const USpaceMapDataAsset* InSpaceMapData)
{
	if (!InSpaceMapData)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UMeteorAvoidanceComponent::MeteorDetect] InSpaceMapData가 nullptr입니다.")
		);
		return ;
	}
	// 운석 데이터가 이상하면 에러 로그 띄운 후 리턴
	if (InSpaceMapData->MeteorAlarmTimer <= 0.0f
		|| InSpaceMapData->MeteorSpeed <= 0.0f)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[UMeteorAvoidanceComponent::MeteorDetect] InSpaceMapData [%s]의 운석 설정 값이 이상합니다."),
			*InSpaceMapData->MapName.ToString()
		);
		return ;
	}
	// 충돌 할 운석에 데이터 세팅하기
	this->TargetMeteor_.MeteorRemainTime = InSpaceMapData->MeteorAlarmTimer;
	this->TargetMeteor_.MeteorDamage = InSpaceMapData->MeteorDamage;
	this->TargetMeteor_.MeteorSpeed = InSpaceMapData->MeteorSpeed;
	this->TargetMeteor_.MeteorSize = InSpaceMapData->MeteorSize * FMath::FRandRange(0.8f, 1.2f);
	this->TargetMeteor_.MoveDir = FVector(
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f)
	);
	this->TargetMeteor_.MoveDir.Normalize();
	this->TargetMeteor_.StartPos = -1.0f * this->TargetMeteor_.MoveDir * InSpaceMapData->ItemSpawnDist;
	this->TargetMeteor_.EndPos = this->TargetMeteor_.MoveDir * InSpaceMapData->ItemSpawnDist;
	this->CollisionRadiusSquared__ = FMath::Square(this->TargetMeteor_.MeteorSize + this->SpaceShipSafeArea__);
	this->bIsAvoid__ = false;
	this->bIsMeteor__ = true;
	// 1초마다 운석 충돌 관련 알람 울리기
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.SetTimer(
		this->MeteorAlarmTimer__,
		this,
		&UMeteorAvoidanceComponent::MeteorAlarm,
		this->MeteorAlarmTime__,
		true,
		0.0f
	);
}

void UMeteorAvoidanceComponent::SpaceShipMoveInput(const FVector2D& InMoveInput, const float InSpaceShipSpeed)
{
	if (!this->bIsMeteor__ || this->bIsAvoid__)
	{
		return ;
	}
	// 우주선 입력과 반대로 운석 위치 조정
	FVector	MeteorMoveDir = FVector(0.0f, -InMoveInput.Y, -InMoveInput.X);

	MeteorMoveDir.Normalize();
	MeteorMoveDir *= InSpaceShipSpeed;
	this->TargetMeteor_.StartPos += MeteorMoveDir;
	this->TargetMeteor_.EndPos += MeteorMoveDir;
	OnMeteorMove.ExecuteIfBound(this->TargetMeteor_);
	this->EvaluateMeteorAvoidance();
}

void UMeteorAvoidanceComponent::EvaluateMeteorAvoidance()
{
	if (!this->bIsMeteor__ || this->bIsAvoid__)
	{
		return;
	}
	// 운석 시작점과 끝점
	FVector&	MeteorStart = TargetMeteor_.StartPos;
	FVector&	MeteorEnd = TargetMeteor_.EndPos;
	// 운석의 궤도와 방향
	FVector		MeteorSegment = MeteorEnd - MeteorStart;
	// 운석 궤도의 길이(제곱)
	float		SegmentLengthSquared = MeteorSegment.SizeSquared();
	// 운석과 우주선이 가장 가까운 거리 비율 (외적)
	float		ClosetT = FVector::CrossProduct(MeteorStart, MeteorSegment).SizeSquared() / SegmentLengthSquared;
	// 가장 가까울 때 위치 업데이트
	this->TargetMeteor_.ClosestApproachPos = MeteorStart + MeteorSegment * ClosetT;
	float		ClosetDistSquared = this->TargetMeteor_.ClosestApproachPos.SizeSquared();

	if (ClosetDistSquared > this->CollisionRadiusSquared__)
	{
		this->ClearMeteor();
	}
}

void UMeteorAvoidanceComponent::MeteorAlarm()
{
	this->TargetMeteor_.MeteorRemainTime -= this->MeteorAlarmTime__;
	if (this->TargetMeteor_.MeteorRemainTime <= 0.0f)
	{
		this->SpawnMeteor();
		return ;
	}
	OnMeteorDetect.ExecuteIfBound();
}

void UMeteorAvoidanceComponent::SpawnMeteor()
{
	if (!this->bIsMeteor__ || this->bIsAvoid__)
	{
		this->TargetMeteor_.Clear();
		return;
	}
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[UMeteorAvoidanceComponent::SpawnMeteor] 운석 회피 실패")
	);
	// 운석 spawn
	OnMeteorCollision.ExecuteIfBound(this->TargetMeteor_);
	this->TargetMeteor_.Clear();
}

void UMeteorAvoidanceComponent::ClearMeteor()
{
	this->TargetMeteor_.Clear();
	this->bIsMeteor__ = false;
	this->bIsAvoid__ = true;
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	TimerManager.ClearTimer(this->MeteorAlarmTimer__);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[UMeteorAvoidanceComponent::SpawnMeteor] 운석 회피 성공")
	);
}
