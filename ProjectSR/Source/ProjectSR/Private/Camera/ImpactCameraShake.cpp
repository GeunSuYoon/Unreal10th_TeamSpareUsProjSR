#include "Camera/ImpactCameraShake.h"

void UImpactCameraShakePattern::Configure(
	float InDuration,
	float InFrequency,
	float InRotationAmplitude,
	float InLocationAmplitude)
{
	Duration__ = InDuration;
	Frequency__ = InFrequency;
	RotationAmplitude__ = InRotationAmplitude;
	LocationAmplitude__ = InLocationAmplitude;
}

void UImpactCameraShakePattern::GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const
{
	OutInfo.Duration = FCameraShakeDuration(Duration__);
	OutInfo.BlendIn = 0.02f;
	OutInfo.BlendOut = FMath::Min(0.18f, Duration__ * 0.5f);
}

void UImpactCameraShakePattern::StartShakePatternImpl(const FCameraShakePatternStartParams& Params)
{
	ElapsedTime__ = 0.0f;
}

void UImpactCameraShakePattern::UpdateShakePatternImpl(
	const FCameraShakePatternUpdateParams& Params,
	FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime__ += Params.DeltaTime;
	EvaluateShake__(ElapsedTime__, OutResult);
}

void UImpactCameraShakePattern::ScrubShakePatternImpl(
	const FCameraShakePatternScrubParams& Params,
	FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime__ = Params.AbsoluteTime;
	EvaluateShake__(ElapsedTime__, OutResult);
}

bool UImpactCameraShakePattern::IsFinishedImpl() const
{
	return ElapsedTime__ >= Duration__;
}

void UImpactCameraShakePattern::StopShakePatternImpl(const FCameraShakePatternStopParams& Params)
{
	ElapsedTime__ = Duration__;
}

void UImpactCameraShakePattern::EvaluateShake__(
	float Time,
	FCameraShakePatternUpdateResult& OutResult) const
{
	const float Progress = FMath::Clamp(Time / Duration__, 0.0f, 1.0f);
	const float Strength = FMath::Square(1.0f - Progress);
	const float Phase = Time * Frequency__ * UE_TWO_PI;

	OutResult.Location = FVector(
		FMath::Sin(Phase * 0.83f) * LocationAmplitude__ * Strength,
		FMath::Sin(Phase * 1.17f + 1.1f) * LocationAmplitude__ * Strength,
		FMath::Sin(Phase * 1.41f + 2.2f) * LocationAmplitude__ * Strength);
	OutResult.Rotation = FRotator(
		FMath::Sin(Phase) * RotationAmplitude__ * Strength,
		FMath::Sin(Phase * 1.29f + 0.7f) * RotationAmplitude__ * Strength,
		FMath::Sin(Phase * 0.71f + 1.8f) * RotationAmplitude__ * 0.5f * Strength);
}

UMeteorImpactCameraShake::UMeteorImpactCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSingleInstance = true;
	UImpactCameraShakePattern* Pattern = ObjectInitializer.CreateDefaultSubobject<UImpactCameraShakePattern>(
		this, TEXT("MeteorImpactPattern"));
	SetRootShakePattern(Pattern);
	Pattern->Configure(0.45f, 24.0f, 1.6f, 2.5f);
}

USolarWindCameraShake::USolarWindCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSingleInstance = true;
	UImpactCameraShakePattern* Pattern = ObjectInitializer.CreateDefaultSubobject<UImpactCameraShakePattern>(
		this, TEXT("SolarWindPattern"));
	SetRootShakePattern(Pattern);
	Pattern->Configure(0.9f, 15.0f, 2.4f, 4.0f);
}
