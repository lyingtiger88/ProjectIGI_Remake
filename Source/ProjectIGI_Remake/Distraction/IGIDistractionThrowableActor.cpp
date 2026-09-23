#include "Distraction/IGIDistractionThrowableActor.h"

#include "Acoustics/BDFRAcousticEventLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AIGIDistractionThrowableActor::AIGIDistractionThrowableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    ThrowableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThrowableMesh"));
    SetRootComponent(ThrowableMesh);

    ThrowableMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    ThrowableMesh->SetGenerateOverlapEvents(false);
    ThrowableMesh->SetNotifyRigidBodyCollision(true);
    ThrowableMesh->SetSimulatePhysics(true);
    ThrowableMesh->SetEnableGravity(true);
    ThrowableMesh->SetLinearDamping(0.08f);
    ThrowableMesh->SetAngularDamping(0.15f);
    ThrowableMesh->OnComponentHit.AddDynamic(this, &ThisClass::HandleHit);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PrototypeMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    if (PrototypeMesh.Succeeded())
    {
        ThrowableMesh->SetStaticMesh(PrototypeMesh.Object);
        ThrowableMesh->SetRelativeScale3D(FVector(0.08f));
    }
}

void AIGIDistractionThrowableActor::InitializeThrow(
    AActor* InThrower,
    const FVector& InitialVelocity,
    UStaticMesh* InMesh)
{
    ThrowerActor = InThrower;
    SetOwner(InThrower);
    SetInstigator(Cast<APawn>(InThrower));

    if (IsValid(InMesh))
    {
        ThrowableMesh->SetStaticMesh(InMesh);
        ThrowableMesh->SetRelativeScale3D(FVector::OneVector);
    }

    if (IsValid(ThrowerActor))
    {
        ThrowableMesh->IgnoreActorWhenMoving(ThrowerActor, true);
    }

    ThrowableMesh->SetPhysicsLinearVelocity(InitialVelocity);
    ThrowableMesh->SetPhysicsAngularVelocityInDegrees(
        FVector(
            FMath::FRandRange(-650.0f, 650.0f),
            FMath::FRandRange(-650.0f, 650.0f),
            FMath::FRandRange(-650.0f, 650.0f)));

    SetLifeSpan(FMath::Max(1.0f, LifeSeconds));
}

void AIGIDistractionThrowableActor::HandleHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    const FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (bReportedDistraction || !IsValid(ThrowableMesh))
    {
        return;
    }

    const float ImpactSpeed = ThrowableMesh->GetPhysicsLinearVelocity().Size();
    const float ImpulseStrength = NormalImpulse.Size();

    if (ImpactSpeed < MinimumImpactSpeed && ImpulseStrength < 8.0f)
    {
        return;
    }

    bReportedDistraction = true;

    const EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(Hit);
    const float SurfaceMultiplier = GetSurfaceNoiseMultiplier(SurfaceType);
    const float SpeedStrength = FMath::Clamp(
        ImpactSpeed / 1250.0f,
        0.35f,
        1.0f);

    const float Loudness = FMath::Clamp(
        BaseImpactLoudness * SurfaceMultiplier * SpeedStrength,
        0.05f,
        1.0f);

    const float HearingRadius = FMath::Max(
        250.0f,
        BaseImpactHearingRadius * SurfaceMultiplier * SpeedStrength);

    // IMPORTANT: the throwable actor is the acoustic instigator, not the player.
    // This lets AI investigate the lure location without treating the player as the
    // confirmed source of the noise.
    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        this,
        Hit.ImpactPoint,
        Loudness,
        HearingRadius,
        GetSurfaceDistractionTag(SurfaceType));

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI distraction impact: %s strength=%.2f radius=%.0f"),
        *GetSurfaceDistractionTag(SurfaceType).ToString(),
        Loudness,
        HearingRadius);
}

float AIGIDistractionThrowableActor::GetSurfaceNoiseMultiplier(
    const EPhysicalSurface SurfaceType)
{
    switch (SurfaceType)
    {
        case SurfaceType2: return 0.55f; // Mud
        case SurfaceType3: return 0.50f; // Snow
        case SurfaceType4: return 0.60f; // Sand
        case SurfaceType5: return 0.65f; // Grass
        case SurfaceType6: return 1.15f; // Concrete
        case SurfaceType7: return 1.45f; // Metal
        case SurfaceType8: return 0.80f; // Water
        case SurfaceType1: return 0.85f; // Dirt
        default: return 1.0f;
    }
}

FName AIGIDistractionThrowableActor::GetSurfaceDistractionTag(
    const EPhysicalSurface SurfaceType)
{
    switch (SurfaceType)
    {
        case SurfaceType2: return TEXT("BDFR.Acoustic.Distraction.Impact.Mud");
        case SurfaceType3: return TEXT("BDFR.Acoustic.Distraction.Impact.Snow");
        case SurfaceType4: return TEXT("BDFR.Acoustic.Distraction.Impact.Sand");
        case SurfaceType5: return TEXT("BDFR.Acoustic.Distraction.Impact.Grass");
        case SurfaceType6: return TEXT("BDFR.Acoustic.Distraction.Impact.Concrete");
        case SurfaceType7: return TEXT("BDFR.Acoustic.Distraction.Impact.Metal");
        case SurfaceType8: return TEXT("BDFR.Acoustic.Distraction.Impact.Water");
        case SurfaceType1: return TEXT("BDFR.Acoustic.Distraction.Impact.Dirt");
        default: return TEXT("BDFR.Acoustic.Distraction.Impact.Generic");
    }
}
