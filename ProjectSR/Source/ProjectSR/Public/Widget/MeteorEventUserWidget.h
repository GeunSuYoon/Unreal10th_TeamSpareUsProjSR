// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonHeader/Meteor.h"

#include "Blueprint/UserWidget.h"
#include "MeteorEventUserWidget.generated.h"

class USizeBox;
class UCanvasPanelSlot;

class ASpaceShipActor;
class UMeteorRouteUserWidget;
/**
 * 
 */
UCLASS()
class PROJECTSR_API UMeteorEventUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void	BindToSpaceShip(ASpaceShipActor* InSpaceShip);

	UFUNCTION()
	void	SetWidgetSize(const float InSapwnDist);

	UFUNCTION()
	void	SetMeteorRoute(const FMeteor& InMeteor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox>	MeteorWidgetSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox>	SpaceShipImageSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox>	MeteorRouteSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMeteorRouteUserWidget>	MeteorRouteWidget;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanelSlot> MeteorRouteCanvasSlot;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void	OnMeteorDetect__(const FMeteor& InMeteor);

	UFUNCTION()
	void	OnMeteorClear__();

	FVector2D	MapCenter2D__ = FVector2D::ZeroVector;

	float	MapSidePx__ = -1.0f;
	float	PixelPerUnit = -1.0f;
	float	WidgetSize__ = -1.0f;
	float	SpaceShipSize__ = -1.0f;
	float	SpaceMapSpawnDist__ = -1.0f;

};
