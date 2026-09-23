#include "Stealth/IGIAcousticSignatureComponent.h"

#include "Acoustics/BDFRAcousticEventLibrary.h"
#include "AlsCharacter.h"
#include "GameFramework/Actor.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Tracking/BDFRTrackingTypes.h"
#include "Tracking/IGITrackingSurfaceComponent.h"

UIGIAcousticSignatureComponent::UIGIAcousticSignatureComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UIGIAcousticSignatureComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    InventoryComponent = IsValid(Owner)
        ? Owner->FindComponentByClass<UIGIInventoryComponent>()
        : nullptr;
    TrackingSurfaceComponent = IsValid(Owner)
        ? Owner->FindComponentByClass<UIGITrackingSurfaceComponent>()
        : nullptr;

    LastLocation = IsValid(Owner) ? Owner->GetActorLocation() : FVector::ZeroVector;
    PrimaryComponentTick.TickInterval = UpdateIntervalSeconds;
}

void UIGIAcousticSignatureComponent::TickComponent(
    const float DeltaTime,
    const ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const FVector CurrentLocation = Owner->GetActorLocation();
    const FVector Delta = CurrentLocation - LastLocation;
    LastLocation = CurrentLocation;

    const float PlanarDelta = FVector(Delta.X, Delta.Y, 0.0f).Size();
    const float Speed = GetCurrentPlanarSpeed();

    if (Speed < MinimumMovementSpeed || PlanarDelta > 500.0f)
    {
        AccumulatedPlanarDistance = 0.0f;
        GearNoiseElapsed = 0.0f;
        return;
    }

    AccumulatedPlanarDistance += PlanarDelta;
    GearNoiseElapsed += DeltaTime;

    if (bAutomaticFootsteps && AccumulatedPlanarDistance >= GetCurrentStepDistance())
    {
        AccumulatedPlanarDistance = FMath::Fmod(
            AccumulatedPlanarDistance,
            FMath::Max(1.0f, GetCurrentStepDistance()));

        ReportFootstep();
    }

    if (GearNoiseElapsed >= GearNoiseIntervalSeconds && GetCurrentLoadNoiseMultiplier() > 1.05f)
    {
        GearNoiseElapsed = 0.0f;
        ReportGearNoise();
    }
}

void UIGIAcousticSignatureComponent::ReportFootstep(const float Intensity)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const float CombinedMultiplier =
        GetMovementSpeedMultiplier() *
        GetStanceMultiplier() *
        GetSurfaceNoiseMultiplier() *
        GetCurrentLoadNoiseMultiplier() *
        FMath::Max(0.0f, Intensity);

    const float Loudness = FMath::Clamp(BaseFootstepLoudness * CombinedMultiplier, 0.05f, 1.0f);
    const float Radius = FMath::Max(0.0f, BaseFootstepRadius * CombinedMultiplier);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        Owner,
        Owner->GetActorLocation(),
        Loudness,
        Radius,
        TEXT("BDFR.Acoustic.Movement.Footstep"));
}

void UIGIAcousticSignatureComponent::ReportGearNoise(const float Intensity)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const float LoadMultiplier = GetCurrentLoadNoiseMultiplier();
    const float LoadExcess = FMath::Max(0.0f, LoadMultiplier - 1.0f);
    const float CombinedMultiplier =
        (1.0f + LoadExcess * 1.5f) *
        GetMovementSpeedMultiplier() *
        GetStanceMultiplier() *
        FMath::Max(0.0f, Intensity);

    const float Loudness = FMath::Clamp(BaseGearLoudness * CombinedMultiplier, 0.05f, 0.75f);
    const float Radius = FMath::Max(0.0f, BaseGearRadius * CombinedMultiplier);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        Owner,
        Owner->GetActorLocation(),
        Loudness,
        Radius,
        TEXT("BDFR.Acoustic.Movement.Gear"));
}

void UIGIAcousticSignatureComponent::ReportLanding(const float Intensity)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const float CombinedMultiplier =
        GetSurfaceNoiseMultiplier() *
        GetCurrentLoadNoiseMultiplier() *
        FMath::Max(0.0f, Intensity);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        Owner,
        Owner->GetActorLocation(),
        FMath::Clamp(0.35f * CombinedMultiplier, 0.05f, 1.0f),
        1800.0f * CombinedMultiplier,
        TEXT("BDFR.Acoustic.Movement.Landing"));
}

void UIGIAcousticSignatureComponent::ReportVault(const float Intensity)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const float CombinedMultiplier =
        GetCurrentLoadNoiseMultiplier() *
        FMath::Max(0.0f, Intensity);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        Owner,
        Owner->GetActorLocation(),
        FMath::Clamp(0.22f * CombinedMultiplier, 0.05f, 0.85f),
        1200.0f * CombinedMultiplier,
        TEXT("BDFR.Acoustic.Movement.Vault"));
}

float UIGIAcousticSignatureComponent::GetCurrentLoadNoiseMultiplier() const
{
    return IsValid(InventoryComponent)
        ? InventoryComponent->GetMovementNoiseMultiplier()
        : 1.0f;
}

float UIGIAcousticSignatureComponent::GetMovementSpeedMultiplier() const
{
    const float Speed = GetCurrentPlanarSpeed();

    if (Speed < 180.0f)
    {
        return 0.55f;
    }

    if (Speed < 350.0f)
    {
        return 0.80f;
    }

    if (Speed < 520.0f)
    {
        return 1.0f;
    }

    return 1.35f;
}

float UIGIAcousticSignatureComponent::GetStanceMultiplier() const
{
    if (const AIGIPlayerCharacter* IGICharacter = Cast<AIGIPlayerCharacter>(GetOwner());
        IsValid(IGICharacter) && IGICharacter->IsProne())
    {
        return ProneNoiseMultiplier;
    }

    const AAlsCharacter* AlsCharacter = Cast<AAlsCharacter>(GetOwner());
    return IsValid(AlsCharacter) && AlsCharacter->GetStance() == AlsStanceTags::Crouching
        ? CrouchNoiseMultiplier
        : 1.0f;
}

float UIGIAcousticSignatureComponent::GetSurfaceNoiseMultiplier() const
{
    if (!IsValid(TrackingSurfaceComponent))
    {
        return 1.0f;
    }

    switch (TrackingSurfaceComponent->GetCurrentTrackSurface())
    {
        case EBDFRTrackSurfaceType::Dirt: return 0.80f;
        case EBDFRTrackSurfaceType::Mud: return 0.75f;
        case EBDFRTrackSurfaceType::Snow: return 1.05f;
        case EBDFRTrackSurfaceType::Sand: return 0.70f;
        case EBDFRTrackSurfaceType::Grass: return 0.60f;
        case EBDFRTrackSurfaceType::Concrete: return 1.15f;
        case EBDFRTrackSurfaceType::Metal: return 1.45f;
        case EBDFRTrackSurfaceType::Water: return 1.35f;
        default: return 1.0f;
    }
}

float UIGIAcousticSignatureComponent::GetCurrentPlanarSpeed() const
{
    const AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return 0.0f;
    }

    const FVector Velocity = Owner->GetVelocity();
    return FVector(Velocity.X, Velocity.Y, 0.0f).Size();
}

float UIGIAcousticSignatureComponent::GetCurrentStepDistance() const
{
    const float Speed = GetCurrentPlanarSpeed();
    const float SpeedScale = FMath::GetMappedRangeValueClamped(
        FVector2D(100.0f, 650.0f),
        FVector2D(0.85f, 1.25f),
        Speed);

    return BaseStepDistanceCm * SpeedScale;
}
