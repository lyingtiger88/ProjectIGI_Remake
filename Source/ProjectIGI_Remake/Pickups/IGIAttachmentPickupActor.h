#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIAttachmentPickupActor.generated.h"

class UPrimitiveComponent;
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

    // Creates a stat-only pistol suppressor at runtime when no binary attachment asset
    // has been authored yet. The production path remains AttachmentData.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype")
    bool bUsePrototypePistolSuppressor = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup|Prototype")
    TSoftObjectPtr<UStaticMesh> PrototypeSuppressorMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    bool bReplaceExistingAttachment = true;

private:
    UPROPERTY(Transient)
    TObjectPtr<UIGIWeaponAttachmentDataAsset> RuntimePrototypeAttachment;

    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    bool TryInstallOnActiveWeapon(AActor* OtherActor);
    UIGIWeaponAttachmentDataAsset* ResolveAttachmentData();
    UIGIWeaponAttachmentDataAsset* CreatePrototypePistolSuppressor();
};
