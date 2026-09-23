#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIFlareSignalWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FIGIWorldFlareSignalSignature,
    AActor*,
    FlareActor,
    EIGIFlarePurpose,
    Purpose,
    FVector,
    Location);

UCLASS(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIFlareSignalWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "IGI|Flare")
    void ReportFlareSignal(AActor* FlareActor, EIGIFlarePurpose Purpose, FVector Location);

    UFUNCTION(BlueprintPure, Category = "IGI|Flare")
    bool HasActiveFlareSignal() const { return bHasSignal; }

    UFUNCTION(BlueprintPure, Category = "IGI|Flare")
    EIGIFlarePurpose GetLastFlarePurpose() const { return LastPurpose; }

    UFUNCTION(BlueprintPure, Category = "IGI|Flare")
    FVector GetLastFlareLocation() const { return LastLocation; }

    UPROPERTY(BlueprintAssignable, Category = "IGI|Flare")
    FIGIWorldFlareSignalSignature OnFlareSignal;

private:
    UPROPERTY(Transient)
    bool bHasSignal = false;

    UPROPERTY(Transient)
    EIGIFlarePurpose LastPurpose = EIGIFlarePurpose::Illumination;

    UPROPERTY(Transient)
    FVector LastLocation = FVector::ZeroVector;
};
