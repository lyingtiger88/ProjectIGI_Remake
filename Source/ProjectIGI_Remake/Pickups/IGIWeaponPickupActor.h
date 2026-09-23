#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIWeaponPickupActor.generated.h"

class AIGIWeaponBase;
class UNiagaraSystem;
class UPrimitiveComponent;
class USphereComponent;
class UStaticMesh;
class UStaticMeshComponent;
class USkeletalMesh;
class UIGIWeaponDataAsset;

UENUM(BlueprintType)
enum class EIGIPrototypeWeaponPreset : uint8
{
    None,
    Glock17
};

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Weapon Pickup"))
class PROJECTIGI_REMAKE_API AIGIWeaponPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIWeaponPickupActor();

    UFUNCTION(BlueprintCallable, Category = "IGI|Pickup")
    bool TryPickupByActor(AActor* OtherActor);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TSubclassOf<AIGIWeaponBase> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UIGIWeaponDataAsset> WeaponData;

    // Development-only convenience so the complete first weapon loop can be tested
    // before binary Data Assets are authored and committed.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype")
    EIGIPrototypeWeaponPreset PrototypePreset = EIGIPrototypeWeaponPreset::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype")
    TSoftObjectPtr<USkeletalMesh> PrototypeWeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype|Shot FX")
    TSoftObjectPtr<UNiagaraSystem> PrototypeMuzzleFlashEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype|Shot FX")
    TSoftObjectPtr<UNiagaraSystem> PrototypeMuzzleSmokeEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype|Shot FX")
    TSoftObjectPtr<UStaticMesh> PrototypeCasingMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    bool bAutoEquip = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Ammo", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 34;

private:
    UPROPERTY(Transient)
    TObjectPtr<UIGIWeaponDataAsset> RuntimePrototypeData;

    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool TryGiveWeaponTo(AActor* OtherActor);
    UIGIWeaponDataAsset* ResolveWeaponData();
    UIGIWeaponDataAsset* CreateGlock17PrototypeData();
};
