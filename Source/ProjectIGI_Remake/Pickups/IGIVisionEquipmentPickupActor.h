#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIVisionEquipmentPickupActor.generated.h"

class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Vision Equipment Pickup"))
class PROJECTIGI_REMAKE_API AIGIVisionEquipmentPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIVisionEquipmentPickupActor();

    UFUNCTION(BlueprintCallable, Category = "IGI|Pickup")
    bool TryPickupByActor(AActor* OtherActor);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    EIGIEquipmentType VisionEquipmentType = EIGIEquipmentType::Binoculars;

private:
    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool IsSupportedVisionEquipment(EIGIEquipmentType EquipmentType) const;
};
