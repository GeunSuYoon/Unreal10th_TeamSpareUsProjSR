#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShip/SpaceShipUpgradeComponent.h"
#include "ShipUpgradeDetailUserWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UManufactureIngredientItemWidget;

// Shared request, material list and event lifecycle for the three existing pages.
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
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UButton> UpgradeButton;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UPanelWidget> IngredientList;
    UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> UpgradeMessage;
    UPROPERTY(EditDefaultsOnly, Category="Upgrade")
    TSubclassOf<UManufactureIngredientItemWidget> IngredientWidgetClass;
    UPROPERTY(Transient) TObjectPtr<USpaceShipUpgradeComponent> UpgradeComponent;
private:
    void EnsureControls();
    void Subscribe();
    static FText GetResultText(EUpgradeResult Result, int32 NextLevel);
};

