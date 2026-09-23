#pragma once

#include "CoreMinimal.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIFirearmBase.generated.h"

class AController;
class UIGIInventoryComponent;

UCLASS(Blueprintable)
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

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    bool FireHitscan(AController* InstigatorController);

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    FVector GetLastShotImpactLocation() const { return LastShotImpactLocation; }

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    bool DidLastShotHit() const { return bLastShotHit; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    bool bStartLoaded = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Fire")
    TEnumAsByte<ECollisionChannel> WeaponTraceChannel = ECC_Visibility;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    int32 CurrentMagazineAmmo = 0;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Ammo")
    EIGIFireMode CurrentFireMode = EIGIFireMode::SemiAutomatic;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Fire")
    FVector LastShotImpactLocation = FVector::ZeroVector;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "IGI|Weapon|Fire")
    bool bLastShotHit = false;

    void ResetRuntimeWeaponState();
};
