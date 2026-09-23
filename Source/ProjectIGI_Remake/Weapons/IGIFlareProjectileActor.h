#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIFlareProjectileActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UPointLightComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMesh;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FIGIFlareActivatedSignature,
    AActor*,
    FlareActor,
    EIGIFlarePurpose,
    Purpose,
    FVector,
    Location);

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Flare Projectile"))
class PROJECTIGI_REMAKE_API AIGIFlareProjectileActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIFlareProjectileActor();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Flare")
    void InitializeFlare(
        EIGIFlarePurpose InPurpose,
        float InitialSpeed,
        float LifeSeconds,
        UStaticMesh* InMesh,
        UNiagaraSystem* InTrailEffect);

    UFUNCTION(BlueprintPure, Category = "IGI|Flare")
    EIGIFlarePurpose GetFlarePurpose() const { return FlarePurpose; }

    UPROPERTY(BlueprintAssignable, Category = "IGI|Flare")
    FIGIFlareActivatedSignature OnFlareActivated;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TObjectPtr<USphereComponent> CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TObjectPtr<UStaticMeshComponent> FlareMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TObjectPtr<UPointLightComponent> FlareLight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Flare|Light")
    float BaseLightIntensity = 18000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Flare|Light")
    float LightAttenuationRadius = 2600.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Flare|Physics")
    float WindAccelerationScale = 0.28f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Flare|Physics")
    float ImpactBounceDamping = 0.20f;

private:
    UPROPERTY(Transient)
    TObjectPtr<UNiagaraComponent> TrailComponent;

    UPROPERTY(Transient)
    EIGIFlarePurpose FlarePurpose = EIGIFlarePurpose::Illumination;

    bool bActivated = false;

    UFUNCTION()
    void HandleProjectileStop(const FHitResult& ImpactResult);

    void ActivateSignal(const FVector& Location);
    void ApplyWeatherToLight();
};
