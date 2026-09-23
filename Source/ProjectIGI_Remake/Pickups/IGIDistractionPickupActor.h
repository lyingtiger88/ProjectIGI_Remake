#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIDistractionPickupActor.generated.h"

class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Distraction Pickup"))
class PROJECTIGI_REMAKE_API AIGIDistractionPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIDistractionPickupActor();

    UFUNCTION(BlueprintCallable, Category = "IGI|Pickup")
    bool TryPickupByActor(AActor* OtherActor);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<USphereComponent> PickupSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Pickup")
    TObjectPtr<UStaticMeshComponent> PickupMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Pickup", meta = (ClampMin = "1"))
    int32 DistractionObjectCount = 1;

private:
    UFUNCTION()
    void HandlePickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};
