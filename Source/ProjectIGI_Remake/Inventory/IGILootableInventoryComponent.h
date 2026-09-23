#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGILootableInventoryComponent.generated.h"

class UIGIInventoryComponent;

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGILootableInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGILootableInventoryComponent();

    UFUNCTION(BlueprintCallable, Category = "IGI|Loot")
    int32 LootAllRemainingAmmunition(AActor* RecipientActor, bool bIncludeLoadedMagazines = true);

    UFUNCTION(BlueprintCallable, Category = "IGI|Loot")
    int32 LootAllRemainingAmmunitionToInventory(
        UIGIInventoryComponent* RecipientInventory,
        bool bIncludeLoadedMagazines = true);

    UFUNCTION(BlueprintPure, Category = "IGI|Loot")
    int32 GetTotalRemainingRounds(bool bIncludeLoadedMagazines = true) const;

    UFUNCTION(BlueprintCallable, Category = "IGI|Loot")
    int32 LootAllMedKits(AActor* RecipientActor);

    UFUNCTION(BlueprintCallable, Category = "IGI|Loot")
    int32 LootAllMedKitsToInventory(UIGIInventoryComponent* RecipientInventory);

    UFUNCTION(BlueprintPure, Category = "IGI|Loot")
    int32 GetRemainingMedKitCount() const;

private:
    UIGIInventoryComponent* GetSourceInventory() const;
};
