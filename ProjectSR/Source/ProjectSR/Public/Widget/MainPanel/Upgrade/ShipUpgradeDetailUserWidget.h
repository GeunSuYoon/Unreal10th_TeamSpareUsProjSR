#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "ShipUpgradeDetailUserWidget.generated.h"

class UButton;
class UTextBlock;
class UUniformGridPanel;
class UManufactureIngredientItemWidget;

// Shared upgrade request, ingredient grid and event lifecycle.
UCLASS(Abstract)
class PROJECTSR_API UShipUpgradeDetailUserWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="Upgrade")
    void BindToUpgradeComponent(USpaceShipUpgradeComponent* InComponent);
    UFUNCTION(BlueprintCallable, Category="Upgrade")
    void Refresh();
    UFUNCTION(BlueprintCallable, Category="Upgrade")
    void HandleUpgradeClicked();
protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual EUpgradeTarget GetUpgradeTarget() const { return EUpgradeTarget::SpaceShip; }
    virtual void RefreshStats(const FShipUpgradePreview& Preview) {}
    static void SetNumber(UTextBlock* Text, float Value, bool bAvailable = true);

    // These names intentionally match UManufactureWidget and the existing upgrade WBPs.
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(BindWidgetOptional))
    TObjectPtr<UUniformGridPanel> IngredientItemGridPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(BindWidgetOptional))
    TObjectPtr<UButton> UpgradeButton;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(BindWidgetOptional))
    TObjectPtr<UTextBlock> UpgradeMessage;

    UPROPERTY(EditDefaultsOnly, Category="Upgrade")
    TSubclassOf<UManufactureIngredientItemWidget> IngredientWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category="Upgrade", meta=(ClampMin="1"))
    int32 MaxIngredientColumnCount = 5;

    UPROPERTY(Transient) TObjectPtr<USpaceShipUpgradeComponent> UpgradeComponent;
private:
    void Subscribe();
    void ResolveIngredientWidgetClassFromDesigner();
    static FText GetResultText(EUpgradeResult Result, int32 NextLevel);
};

