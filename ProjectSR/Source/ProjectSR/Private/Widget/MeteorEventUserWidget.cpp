// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MeteorEventUserWidget.h"
#include "Widget/MeteorRouteUserWidget.h"
#include "SpaceShip/SpaceShipActor.h"
#include "SpaceShip/MeteorAvoidanceComponent.h"
#include "Framework/Subsystem/SpaceSalvageWorldSubsystem.h"

#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/OverlaySlot.h"

void UMeteorEventUserWidget::BindToSpaceShip(ASpaceShipActor* InSpaceShip)
{
	UMeteorAvoidanceComponent* Component = InSpaceShip->GetMeteorAvoidanceComponent();

	this->SpaceShipSize__ = InSpaceShip->GetSafeAreaRadius();
	Component->OnMeteorDetect.AddDynamic(this, &UMeteorEventUserWidget::OnMeteorDetect__);
	Component->OnMeteorMove.AddDynamic(this, &UMeteorEventUserWidget::SetMeteorRoute);
	Component->OnMeteorClear.AddDynamic(this, &UMeteorEventUserWidget::OnMeteorClear__);
	USpaceSalvageWorldSubsystem* SpaceSubsystem = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();

	if (IsValid(SpaceSubsystem))
	{
		SpaceSubsystem->OnSpaceMapUpdate.BindUFunction(this, TEXT("SetWidgetSize"));
		if (SpaceSubsystem->GetSpaceMapData())
		{
			this->SetWidgetSize(SpaceSubsystem->GetItemSpawnDist());

		}
	}
}

void UMeteorEventUserWidget::SetWidgetSize(const float InSapwnDist)
{
	this->PixelPerUnit = this->MapSidePx__ / (2.0f * InSapwnDist);
	this->SpaceMapSpawnDist__ = InSapwnDist;
	float	SpaceShipWidgetScale = PixelPerUnit * 2.0f * this->SpaceShipSize__ / 100.0f;

	this->SpaceShipImageSize->SetRenderScale(FVector2D(SpaceShipWidgetScale));
}

void UMeteorEventUserWidget::SetMeteorRoute(const FMeteor& InMeteor)
{
	if (!this->MeteorRouteCanvasSlot)
	{
		return;
	}
	FVector	RouteCenter3D = (InMeteor.StartPos + InMeteor.EndPos) * 0.5f;
	FVector	RouteDirection3D = InMeteor.MoveDir;

	FVector2D	RouteCenter2D = this->MapCenter2D__ + FVector2D(-RouteCenter3D.Y, RouteCenter3D.Z) * this->PixelPerUnit;
	FVector2D	RouteDirection2D = FVector2D(-RouteDirection3D.Y, RouteDirection3D.Z);
	float		RouteAngle = FMath::RadiansToDegrees(
		FMath::Atan2(
			RouteDirection2D.Y,
			RouteDirection2D.X
		)) + 90.0f;

	this->MeteorRouteSize->SetWidthOverride(this->PixelPerUnit * InMeteor.MeteorSize);
	this->MeteorRouteSize->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	this->MeteorRouteSize->SetRenderTransformAngle(RouteAngle);
	this->MeteorRouteCanvasSlot->SetPosition(RouteCenter2D);
}

void UMeteorEventUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	USpaceSalvageWorldSubsystem* SpaceSubsystem = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (IsValid(SpaceSubsystem))
	{
		SpaceSubsystem->OnSpaceMapUpdate.BindUFunction(this, TEXT("SetWidgetSize"));
	}
	this->MeteorRouteCanvasSlot = Cast<UCanvasPanelSlot>(MeteorRouteSize->Slot);
	if (!this->MeteorRouteCanvasSlot)
	{
		return ;
	}
	this->MeteorRouteCanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	this->MeteorRouteCanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	if (this->MeteorWidgetSize)
	{
		this->MapSidePx__ = this->MeteorWidgetSize->GetWidthOverride();
		if (this->MeteorWidgetSize->GetHeightOverride() < this->MapSidePx__)
		{
			this->MapSidePx__ = this->MeteorWidgetSize->GetHeightOverride();
		}
		this->MapCenter2D__ = FVector2D(this->MapSidePx__ * 0.5f, this->MapSidePx__ * 0.5f);
	}
}

void UMeteorEventUserWidget::OnMeteorDetect__(const FMeteor& InMeteor)
{
	USpaceSalvageWorldSubsystem* SpaceSubsystem = GetWorld()->GetSubsystem<USpaceSalvageWorldSubsystem>();
	if (IsValid(SpaceSubsystem))
	{
		this->SetWidgetSize(SpaceSubsystem->GetItemSpawnDist());
	}
	this->MeteorRouteWidget->SetVisibleToggle(true);
	this->SetMeteorRoute(InMeteor);
}

void UMeteorEventUserWidget::OnMeteorClear__()
{
	this->MeteorRouteWidget->SetVisibleToggle(false);
}
