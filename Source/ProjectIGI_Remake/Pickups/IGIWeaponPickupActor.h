#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIWeaponPickupActor.generated.h"

class AIGIWeaponBase;
class USphereComponent;
class UStaticMeshComponent;
class UIGIWeaponDataAsset;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Weapon Pickup"))
class PROJECTIGI_REMAKE_API AIGIWeaponPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIWeaponPickupActor();

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    bool bAutoEquip = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Ammo", meta = (ClampMin = "0"))
    int32 InitialReserveAmmo = 34;

private:
    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool TryGiveWeaponTo(AActor* OtherActor);
};
