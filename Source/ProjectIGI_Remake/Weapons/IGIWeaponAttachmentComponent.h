#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIWeaponAttachmentComponent.generated.h"

class AIGIWeaponBase;
class UIGIWeaponAttachmentDataAsset;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FIGIWeaponAttachmentChangedSignature,
    EIGIAttachmentSlot,
    Slot,
    UIGIWeaponAttachmentDataAsset*,
    Attachment);

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIWeaponAttachmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIWeaponAttachmentComponent();

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    bool CanInstallAttachment(const UIGIWeaponAttachmentDataAsset* Attachment) const;

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Attachments")
    bool InstallAttachment(UIGIWeaponAttachmentDataAsset* Attachment, bool bReplaceExisting = true);

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Attachments")
    bool RemoveAttachment(EIGIAttachmentSlot Slot);

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    UIGIWeaponAttachmentDataAsset* GetAttachment(EIGIAttachmentSlot Slot) const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    bool HasSuppressor() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetRecoilMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetSpreadMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetAimSpeedMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetReloadTimeMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetGunshotNoiseMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetMovementNoiseMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetMuzzleFlashMultiplier() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    int32 GetMagazineCapacityBonus() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Attachments")
    float GetAttachmentWeightKg() const;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Weapon|Attachments")
    FIGIWeaponAttachmentChangedSignature OnAttachmentChanged;

private:
    UPROPERTY(Transient)
    TMap<EIGIAttachmentSlot, TObjectPtr<UIGIWeaponAttachmentDataAsset>> InstalledAttachments;

    UPROPERTY(Transient)
    TMap<EIGIAttachmentSlot, TObjectPtr<UStaticMeshComponent>> VisualComponents;

    AIGIWeaponBase* GetOwnerWeapon() const;
    void RefreshVisual(EIGIAttachmentSlot Slot);
    void DestroyVisual(EIGIAttachmentSlot Slot);
    static FName GetDefaultSocketName(EIGIAttachmentSlot Slot);
};
