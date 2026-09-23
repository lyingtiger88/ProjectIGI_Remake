#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIWeaponBase.generated.h"

class UIGIWeaponAttachmentComponent;
class UIGIWeaponDataAsset;
class USceneComponent;
class USkeletalMeshComponent;

UCLASS(Abstract, Blueprintable)
class PROJECTIGI_REMAKE_API AIGIWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AIGIWeaponBase();

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    virtual void InitializeFromData(UIGIWeaponDataAsset* InWeaponData);

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    UIGIWeaponDataAsset* GetWeaponData() const { return WeaponData; }

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    UIGIWeaponAttachmentComponent* GetAttachmentComponent() const { return AttachmentComponent; }

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    void EquipTo(AActor* NewOwner, USceneComponent* Parent, FName SocketName);

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    void HolsterTo(AActor* NewOwner, USceneComponent* Parent, FName SocketName);

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon")
    void DropWeapon();

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Acoustics")
    bool IsSuppressed() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Acoustics")
    float GetEffectiveShotHearingRadius() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Acoustics")
    float GetCarryNoiseContribution() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    float GetTotalWeightKg() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    float GetEffectiveRecoil() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    float GetEffectiveSpread() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon")
    float GetEffectiveMuzzleFlashScale() const;

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Acoustics")
    void ReportGunshotNoise();

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Acoustics")
    void ReportEquipNoise();

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Acoustics")
    void ReportReloadNoise();

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Acoustics")
    void ReportAttachmentHandlingNoise();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Weapon")
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Weapon")
    TObjectPtr<UIGIWeaponAttachmentComponent> AttachmentComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Weapon")
    TObjectPtr<UIGIWeaponDataAsset> WeaponData;

    FVector GetAcousticLocation() const;
    AActor* GetAcousticInstigator() const;
};
