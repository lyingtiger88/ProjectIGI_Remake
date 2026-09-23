#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIWeaponAttachmentDataAsset.generated.h"

class UStaticMesh;

UCLASS(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIWeaponAttachmentDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    FName AttachmentId = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    EIGIAttachmentSlot AttachmentSlot = EIGIAttachmentSlot::Muzzle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identity")
    EIGIAttachmentType AttachmentType = EIGIAttachmentType::Special;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    FName AttachSocketName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compatibility")
    TArray<EIGIWeaponId> CompatibleWeaponIds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compatibility")
    TArray<EIGIWeaponFamily> CompatibleWeaponFamilies;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compatibility")
    bool bUniversalWhenCompatibilityEmpty = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float RecoilMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float SpreadMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float AimSpeedMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float ReloadTimeMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float GunshotNoiseMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.01"))
    float MovementNoiseMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.0"))
    float MuzzleFlashMultiplier = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0"))
    int32 MagazineCapacityBonus = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Modifiers", meta = (ClampMin = "0.0"))
    float WeightKg = 0.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Suppressor")
    bool bSuppressesWeapon = false;
};
