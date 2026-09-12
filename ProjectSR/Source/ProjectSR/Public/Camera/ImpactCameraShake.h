#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "ImpactCameraShake.generated.h"

UCLASS()
class PROJECTSR_API UImpactCameraShakePattern : public UCameraShakePattern
{
	GENERATED_BODY()

public:
	void Configure(float InDuration, float InFrequency, float InRotationAmplitude, float InLocationAmplitude);

private:
	virtual void GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const override;
	virtual void StartShakePatternImpl(const FCameraShakePatternStartParams& Params) override;
	virtual void UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params,
		FCameraShakePatternUpdateResult& OutResult) override;
	virtual void ScrubShakePatternImpl(const FCameraShakePatternScrubParams& Params,
		FCameraShakePatternUpdateResult& OutResult) override;
	virtual bool IsFinishedImpl() const override;
	virtual void StopShakePatternImpl(const FCameraShakePatternStopParams& Params) override;

	void EvaluateShake__(float Time, FCameraShakePatternUpdateResult& OutResult) const;

	float Duration__ = 0.4f;
	float Frequency__ = 24.0f;
	float RotationAmplitude__ = 1.5f;
	float LocationAmplitude__ = 2.0f;
	float ElapsedTime__ = 0.0f;
};

UCLASS()
class PROJECTSR_API UMeteorImpactCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UMeteorImpactCameraShake(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class PROJECTSR_API USolarWindCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	USolarWindCameraShake(const FObjectInitializer& ObjectInitializer);
};
