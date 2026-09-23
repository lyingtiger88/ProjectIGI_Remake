#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIDistractionThrowableActor.generated.h"

class UPrimitiveComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Distraction Throwable"))
class PROJECTIGI_REMAKE_API AIGIDistractionThrowableActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIDistractionThrowableActor();

    UFUNCTION(BlueprintCallable, Category = "IGI|Distraction")
    void InitializeThrow(
        AActor* InThrower,
        const FVector& InitialVelocity,
        UStaticMesh* InMesh = nullptr);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Distraction")
    TObjectPtr<UStaticMeshComponent> ThrowableMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Distraction|Acoustics", meta = (ClampMin = "0.05", ClampMax = "1.0"))
    float BaseImpactLoudness = 0.62f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Distraction|Acoustics", meta = (ClampMin = "100.0", ForceUnits = "cm"))
    float BaseImpactHearingRadius = 2800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Distraction|Acoustics", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
    float MinimumImpactSpeed = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Distraction", meta = (ClampMin = "1.0", ForceUnits = "s"))
    float LifeSeconds = 20.0f;

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> ThrowerActor;

    bool bReportedDistraction = false;

    UFUNCTION()
    void HandleHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit);

    static float GetSurfaceNoiseMultiplier(EPhysicalSurface SurfaceType);
    static FName GetSurfaceDistractionTag(EPhysicalSurface SurfaceType);
};
