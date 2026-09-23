#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIAttachmentPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UIGIWeaponAttachmentDataAsset;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Attachment Pickup"))
class PROJECTIGI_REMAKE_API AIGIAttachmentPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIAttachmentPickupActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UIGIWeaponAttachmentDataAsset> AttachmentData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    bool bReplaceExistingAttachment = true;

private:
    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool TryInstallOnActiveWeapon(AActor* OtherActor);
};
