#pragma once
#include "CoreMinimal.h"
#include "CommonHeader/SpaceShipStruct.h"
#include "Components/ActorComponent.h"
#include "SpaceShipUpgradeComponent.generated.h"

class UInventoryComponent;
class UCraftingComponent;
UENUM(BlueprintType)
enum class EUpgradeTarget : uint8 { SpaceShip, Lazer, MachineArm };
UENUM(BlueprintType)
enum class EUpgradeResult : uint8 { Success, MaxLevel, NotEnoughIngredients, RequiresHigherShipLevel, InvalidData, Busy };

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpgradeStateChanged);

USTRUCT(BlueprintType)
struct FShipUpgradePreview
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)	EUpgradeResult Result = EUpgradeResult::InvalidData;
    UPROPERTY(BlueprintReadOnly)	int32 CurrentLevel = 0;
    UPROPERTY(BlueprintReadOnly)	int32 NextLevel = 0;
    UPROPERTY(BlueprintReadOnly)	bool bHasNextLevel = false;
    UPROPERTY(BlueprintReadOnly)	TArray<FIngredient> Ingredients;
    UPROPERTY(BlueprintReadOnly)	FSpaceShipStat CurrentShip;
    UPROPERTY(BlueprintReadOnly)	FSpaceShipStat NextShip;
    UPROPERTY(BlueprintReadOnly)	FLazerStat CurrentLazer;
    UPROPERTY(BlueprintReadOnly)	FLazerStat NextLazer;
    UPROPERTY(BlueprintReadOnly)	FMachineArmStat CurrentArm;
    UPROPERTY(BlueprintReadOnly)	FMachineArmStat NextArm;
    UPROPERTY(BlueprintReadOnly)	TArray<FName> UnlockRecipeIds;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTSR_API USpaceShipUpgradeComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    USpaceShipUpgradeComponent();

    // New-game initialization only. Repeated successful calls do not refill the ship.
    UFUNCTION(BlueprintCallable, Category="Upgrade|Initialization")
    bool InitializeLevelZero();

    // Use the same crafting component as the crafting screen.
    UFUNCTION(BlueprintCallable, Category="Upgrade")
    void SetCraftingComponent(UCraftingComponent* InComponent);

    UFUNCTION(BlueprintCallable, Category="Upgrade")
    void SetInventories(const TArray<UInventoryComponent*>& InInventories);

    bool HasConfiguredInventories() const { return !Inventories.IsEmpty(); }

    UFUNCTION(BlueprintPure, Category="Upgrade")
    FShipUpgradePreview GetPreview(EUpgradeTarget Target) const;

    UFUNCTION(BlueprintCallable, Category="Upgrade")
    EUpgradeResult TryUpgrade(EUpgradeTarget Target);

    UFUNCTION(BlueprintPure, Category="Upgrade")
    int32 GetOwnedIngredientCount(UItemDataAsset* ItemData) const;

    UFUNCTION(BlueprintCallable, Category="Upgrade")
    bool RestoreRecipeUnlocks();

    bool SpaceShipUpgrade() { return TryUpgrade(EUpgradeTarget::SpaceShip) == EUpgradeResult::Success; }
    bool LazerUpgrade() { return TryUpgrade(EUpgradeTarget::Lazer) == EUpgradeResult::Success; }
    bool MachineArmUpgrade() { return TryUpgrade(EUpgradeTarget::MachineArm) == EUpgradeResult::Success; }
    bool HasEnoughIngredients(const TArray<FIngredient>& Ingredients, const TArray<UInventoryComponent*>& InInventories);
    bool TryConsumeIngredients(const TArray<FIngredient>& Ingredients, const TArray<UInventoryComponent*>& InInventories);
  
	UPROPERTY(BlueprintAssignable, Category="Upgrade")
	FOnUpgradeStateChanged OnUpgradeStateChanged;
protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Upgrade")
	TObjectPtr<UDataTable> SpaceShipUpgradeTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Upgrade")
	TObjectPtr<UDataTable> LazerUpgradeTable;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Upgrade")
	TObjectPtr<UDataTable> MachineArmUpgradeTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Upgrade")
	TObjectPtr<UCraftingComponent> CraftingComponent;
private:
    UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex);

    UPROPERTY(Transient)
	TArray<TObjectPtr<UInventoryComponent>> Inventories;

    bool bUpgradeInProgress = false;
    bool bLevelZeroInitialized = false;
    TArray<UInventoryComponent*> GetInventories() const;
};

