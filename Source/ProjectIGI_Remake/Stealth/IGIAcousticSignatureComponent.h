#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGIAcousticSignatureComponent.generated.h"

class UIGIInventoryComponent;
class UIGITrackingSurfaceComponent;

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIAcousticSignatureComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIAcousticSignatureComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Stealth|Acoustics")
    void ReportFootstep(float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "IGI|Stealth|Acoustics")
    void ReportGearNoise(float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "IGI|Stealth|Acoustics")
    void ReportLanding(float Intensity = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "IGI|Stealth|Acoustics")
    void ReportVault(float Intensity = 1.0f);

    UFUNCTION(BlueprintPure, Category = "IGI|Stealth|Acoustics")
    float GetCurrentLoadNoiseMultiplier() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics")
    bool bAutomaticFootsteps = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.02"))
    float UpdateIntervalSeconds = 0.10f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "20.0"))
    float BaseStepDistanceCm = 115.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BaseFootstepLoudness = 0.18f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0"))
    float BaseFootstepRadius = 1200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BaseGearLoudness = 0.08f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0"))
    float BaseGearRadius = 700.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.2"))
    float GearNoiseIntervalSeconds = 1.10f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float CrouchNoiseMultiplier = 0.50f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stealth|Acoustics", meta = (ClampMin = "0.0"))
    float MinimumMovementSpeed = 20.0f;

private:
    UPROPERTY(Transient)
    TObjectPtr<UIGIInventoryComponent> InventoryComponent;

    UPROPERTY(Transient)
    TObjectPtr<UIGITrackingSurfaceComponent> TrackingSurfaceComponent;

    FVector LastLocation = FVector::ZeroVector;
    float AccumulatedPlanarDistance = 0.0f;
    float GearNoiseElapsed = 0.0f;

    float GetMovementSpeedMultiplier() const;
    float GetStanceMultiplier() const;
    float GetSurfaceNoiseMultiplier() const;
    float GetCurrentPlanarSpeed() const;
    float GetCurrentStepDistance() const;
};
