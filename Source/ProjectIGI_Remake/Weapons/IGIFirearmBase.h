#pragma once

#include "CoreMinimal.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIFirearmBase.generated.h"

class UIGIInventoryComponent;

UCLASS(Abstract, Blueprintable)
class PROJECTIGI_REMAKE_API AIGIFirearmBase : public AIGIWeaponBase
{
    GENERATED_BODY()

public:
    AIGIFirearmBase();

    virtual void BeginPlay() override;
    virtual void InitializeFromData(UIGIWeaponDataAsset* InWeaponData) override;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Ammo")
    int32 GetCurrentMagazineAmmo() const { return CurrentMagazineAmmo; }

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Ammo")
    int32 GetMagazineCapacity() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Ammo")
    EIGIFireMode GetCurrentFireMode() const { return CurrentFireMode; }

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Ammo")
    bool SetFireMode(EIGIFireMode NewFireMode);

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Ammo")
    bool TryConsumeRound();

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Ammo")
    int32 ReloadFromInventory(UIGIInventoryComponent* Inventory);

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    bool NotifyShotFired();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    bool bStartLoaded = true;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    int32 CurrentMagazineAmmo = 0;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    EIGIFireMode CurrentFireMode = EIGIFireMode::SemiAutomatic;

    void ResetRuntimeWeaponState();
};
