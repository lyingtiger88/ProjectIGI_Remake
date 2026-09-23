#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment/IGIWeatherTypes.h"
#include "IGIShellCasingActor.generated.h"

class UPrimitiveComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Shell Casing"))
class PROJECTIGI_REMAKE_API AIGIShellCasingActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIShellCasingActor();

    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Weapon|Casing")
    void InitializeCasing(
        UStaticMesh* InCasingMesh,
        const FVector& InitialVelocity,
        const FVector& InitialAngularVelocityDegrees,
        float LifeSeconds);

    UFUNCTION(BlueprintPure, Category = "IGI|Weapon|Casing")
    EIGIWeatherType GetSpawnWeatherType() const { return SpawnWeatherState.WeatherType; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Weapon|Casing")
    TObjectPtr<UStaticMeshComponent> CasingMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Casing")
    float WindForceScale = 0.018f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Casing|Acoustics", meta = (ClampMin = "0"))
    int32 MaxAudibleImpacts = 2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IGI|Weapon|Casing|Acoustics", meta = (ClampMin = "0.0"))
    float MinimumAudibleImpactImpulse = 12.0f;

private:
    FIGIWeatherState SpawnWeatherState;
    float WeatherWindResponse = 0.0f;
    int32 AudibleImpactCount = 0;

    UFUNCTION()
    void HandleCasingHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit);

    void ApplyWeatherProfile();
    float GetWeatherImpactNoiseMultiplier() const;
    static float GetSurfaceImpactNoiseMultiplier(EPhysicalSurface SurfaceType);
};
