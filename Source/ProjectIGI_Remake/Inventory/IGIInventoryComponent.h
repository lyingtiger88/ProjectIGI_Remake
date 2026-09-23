#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIInventoryComponent.generated.h"

class AIGIWeaponBase;
class UIGICombatComponent;

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIInventoryComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory")
    bool TryStoreWeapon(AIGIWeaponBase* Weapon, EIGICarrySlot& OutSlot);

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory")
    bool RemoveWeapon(AIGIWeaponBase* Weapon);

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory")
    bool EquipWeaponInSlot(EIGICarrySlot Slot);

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory")
    bool UnequipActiveWeapon();

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory")
    AIGIWeaponBase* GetWeaponInSlot(EIGICarrySlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory")
    AIGIWeaponBase* GetActiveWeapon() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory")
    FName GetSocketNameForSlot(EIGICarrySlot Slot) const;

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory|Ammo")
    int32 AddAmmo(FName AmmoType, int32 Amount, int32 MaxCarry);

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory|Ammo")
    int32 ConsumeAmmo(FName AmmoType, int32 Amount);

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory|Ammo")
    int32 GetAmmoCount(FName AmmoType) const;

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory|Equipment")
    int32 AddEquipment(EIGIEquipmentType EquipmentType, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "IGI|Inventory|Equipment")
    int32 ConsumeEquipment(EIGIEquipmentType EquipmentType, int32 Amount);

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory|Equipment")
    int32 GetEquipmentCount(EIGIEquipmentType EquipmentType) const;

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory|Carry")
    float GetTotalCarryWeightKg() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Inventory|Carry")
    float GetMovementNoiseMultiplier() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Limits", meta = (ClampMin = "0"))
    int32 MaxFragGrenades = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Limits", meta = (ClampMin = "0"))
    int32 MaxFlashbangs = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Limits", meta = (ClampMin = "0"))
    int32 MaxSmokeGrenades = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Limits", meta = (ClampMin = "0"))
    int32 MaxUtilityExplosives = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Carry")
    float FreeWeightThresholdKg = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Inventory|Carry")
    float NoisePerKgAboveThreshold = 0.015f;

private:
    UPROPERTY(Transient)
    TMap<EIGICarrySlot, TObjectPtr<AIGIWeaponBase>> StoredWeapons;

    UPROPERTY(Transient)
    TMap<FName, int32> AmmoPools;

    UPROPERTY(Transient)
    TMap<EIGIEquipmentType, int32> EquipmentCounts;

    UPROPERTY(Transient)
    TObjectPtr<UIGICombatComponent> CombatComponent;

    UPROPERTY(Transient)
    EIGICarrySlot ActiveWeaponSlot = EIGICarrySlot::Weapon01;

    UPROPERTY(Transient)
    bool bHasActiveWeaponSlot = false;

    bool FindFreeCompatibleSlot(const AIGIWeaponBase* Weapon, EIGICarrySlot& OutSlot) const;
    bool IsPhysicalWeaponSlot(EIGICarrySlot Slot) const;
    int32 GetEquipmentLimit(EIGIEquipmentType EquipmentType) const;
    int32 GetUtilityExplosiveTotal() const;
    void AttachStoredWeapon(AIGIWeaponBase* Weapon, EIGICarrySlot Slot) const;
};
