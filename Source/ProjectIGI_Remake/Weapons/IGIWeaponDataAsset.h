#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIWeaponDataAsset.generated.h"

class USkeletalMesh;

UCLASS(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    EIGIWeaponId WeaponId = EIGIWeaponId::None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    EIGIWeaponFamily WeaponFamily = EIGIWeaponFamily::Pistol;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    EIGIHandlingProfile HandlingProfile = EIGIHandlingProfile::Pistol;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<USkeletalMesh> WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    FName EquippedSocket = TEXT("SCK_Weapon_Hand_R");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    FName MuzzleSocket = TEXT("Muzzle");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Carry")
    TArray<EIGICarrySlot> CompatibleCarrySlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Carry", meta = (ClampMin = "0.0"))
    float WeightKg = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Carry", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CarryNoiseContribution = 0.08f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachments")
    TArray<EIGIAttachmentSlot> SupportedAttachmentSlots;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
    FName AmmoType = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
    int32 MagazineCapacity = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0"))
    int32 MaxReserveAmmo = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
    TArray<EIGIFireMode> SupportedFireModes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
    EIGIFireMode DefaultFireMode = EIGIFireMode::SemiAutomatic;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire", meta = (ClampMin = "0.0"))
    float BaseDamage = 25.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire", meta = (ClampMin = "0.0"))
    float EffectiveRangeCm = 5000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Handling", meta = (ClampMin = "0.0"))
    float BaseRecoil = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Handling", meta = (ClampMin = "0.0"))
    float BaseSpread = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Handling", meta = (ClampMin = "0.01"))
    float AimSpeedMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (ClampMin = "0.0"))
    float BaseShotHearingRadius = 6000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float EquipNoiseLoudness = 0.12f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (ClampMin = "0.0"))
    float EquipNoiseRadius = 750.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ReloadNoiseLoudness = 0.16f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (ClampMin = "0.0"))
    float ReloadNoiseRadius = 900.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics")
    bool bIntegralSuppressor = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Acoustics", meta = (EditCondition = "bIntegralSuppressor", ClampMin = "0.05", ClampMax = "1.0"))
    float IntegralSuppressorNoiseMultiplier = 0.35f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual", meta = (ClampMin = "0.0"))
    float BaseMuzzleFlashScale = 1.0f;
};
