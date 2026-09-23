#include "Weapons/IGIShellCasingActor.h"

#include "Acoustics/BDFRAcousticEventLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Environment/IGIWeatherWorldSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AIGIShellCasingActor::AIGIShellCasingActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
    SetRootComponent(CasingMesh);

    CasingMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    CasingMesh->SetGenerateOverlapEvents(false);
    CasingMesh->SetNotifyRigidBodyCollision(true);
    CasingMesh->SetSimulatePhysics(true);
    CasingMesh->SetEnableGravity(true);
    CasingMesh->SetLinearDamping(0.08f);
    CasingMesh->SetAngularDamping(0.12f);
    CasingMesh->OnComponentHit.AddDynamic(this, &ThisClass::HandleCasingHit);
}

void AIGIShellCasingActor::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld(); IsValid(World))
    {
        if (const UIGIWeatherWorldSubsystem* Weather =
                World->GetSubsystem<UIGIWeatherWorldSubsystem>();
            IsValid(Weather))
        {
            SpawnWeatherState = Weather->GetWeatherState();
        }
    }

    ApplyWeatherProfile();
}

void AIGIShellCasingActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!IsValid(CasingMesh) || !CasingMesh->IsSimulatingPhysics() || WeatherWindResponse <= 0.0f)
    {
        return;
    }

    if (!CasingMesh->RigidBodyIsAwake(NAME_None))
    {
        SetActorTickEnabled(false);
        return;
    }

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    const UIGIWeatherWorldSubsystem* Weather =
        World->GetSubsystem<UIGIWeatherWorldSubsystem>();

    if (!IsValid(Weather))
    {
        return;
    }

    const FVector WindVelocity = Weather->GetWindVelocity();
    if (WindVelocity.IsNearlyZero())
    {
        return;
    }

    const FVector CurrentVelocity = CasingMesh->GetPhysicsLinearVelocity();
    const FVector RelativeWind = WindVelocity - CurrentVelocity;
    const float MassKg = FMath::Max(CasingMesh->GetMass(), 0.001f);

    CasingMesh->AddForce(
        RelativeWind * MassKg * WindForceScale * WeatherWindResponse,
        NAME_None,
        false);
}

void AIGIShellCasingActor::InitializeCasing(
    UStaticMesh* InCasingMesh,
    const FVector& InitialVelocity,
    const FVector& InitialAngularVelocityDegrees,
    const float LifeSeconds)
{
    if (IsValid(CasingMesh) && IsValid(InCasingMesh))
    {
        CasingMesh->SetStaticMesh(InCasingMesh);
    }

    SetLifeSpan(FMath::Max(0.5f, LifeSeconds));

    if (IsValid(CasingMesh))
    {
        float VelocityMultiplier = 1.0f;

        switch (SpawnWeatherState.WeatherType)
        {
            case EIGIWeatherType::Rain:
                VelocityMultiplier = 0.92f;
                break;

            case EIGIWeatherType::Snow:
                VelocityMultiplier = 0.78f;
                break;

            case EIGIWeatherType::Storm:
                VelocityMultiplier = 0.94f;
                break;

            case EIGIWeatherType::Sandstorm:
                VelocityMultiplier = 0.86f;
                break;

            case EIGIWeatherType::Clear:
            default:
                VelocityMultiplier = 1.0f;
                break;
        }

        CasingMesh->SetPhysicsLinearVelocity(InitialVelocity * VelocityMultiplier);
        CasingMesh->SetPhysicsAngularVelocityInDegrees(
            InitialAngularVelocityDegrees * VelocityMultiplier);
        CasingMesh->WakeAllRigidBodies();
    }
}

void AIGIShellCasingActor::ApplyWeatherProfile()
{
    if (!IsValid(CasingMesh))
    {
        return;
    }

    switch (SpawnWeatherState.WeatherType)
    {
        case EIGIWeatherType::Rain:
            CasingMesh->SetLinearDamping(0.18f + SpawnWeatherState.SurfaceWetness * 0.18f);
            CasingMesh->SetAngularDamping(0.24f + SpawnWeatherState.SurfaceWetness * 0.20f);
            WeatherWindResponse = 0.55f;
            break;

        case EIGIWeatherType::Snow:
            CasingMesh->SetLinearDamping(0.30f + SpawnWeatherState.SnowDepthFactor * 0.35f);
            CasingMesh->SetAngularDamping(0.38f + SpawnWeatherState.SnowDepthFactor * 0.40f);
            WeatherWindResponse = 0.35f;
            break;

        case EIGIWeatherType::Storm:
            CasingMesh->SetLinearDamping(0.16f);
            CasingMesh->SetAngularDamping(0.22f);
            WeatherWindResponse = 1.0f;
            break;

        case EIGIWeatherType::Sandstorm:
            CasingMesh->SetLinearDamping(0.24f);
            CasingMesh->SetAngularDamping(0.32f);
            WeatherWindResponse = 1.20f;
            break;

        case EIGIWeatherType::Clear:
        default:
            CasingMesh->SetLinearDamping(0.08f);
            CasingMesh->SetAngularDamping(0.12f);
            WeatherWindResponse = 0.12f;
            break;
    }
}


void AIGIShellCasingActor::HandleCasingHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    const FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (AudibleImpactCount >= MaxAudibleImpacts)
    {
        return;
    }

    const float ImpactImpulse = NormalImpulse.Size();
    if (ImpactImpulse < MinimumAudibleImpactImpulse)
    {
        return;
    }

    ++AudibleImpactCount;

    const EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(Hit);
    const float SurfaceMultiplier = GetSurfaceImpactNoiseMultiplier(SurfaceType);
    const float WeatherMultiplier = GetWeatherImpactNoiseMultiplier();
    const float ImpactStrength = FMath::Clamp(ImpactImpulse / 120.0f, 0.25f, 1.0f);

    const float Loudness = FMath::Clamp(
        0.11f * ImpactStrength * SurfaceMultiplier * WeatherMultiplier,
        0.05f,
        0.45f);

    const float HearingRadius = FMath::Clamp(
        650.0f * ImpactStrength * SurfaceMultiplier * WeatherMultiplier,
        250.0f,
        1200.0f);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        GetOwner(),
        Hit.ImpactPoint,
        Loudness,
        HearingRadius,
        TEXT("BDFR.Acoustic.Weapon.CasingImpact"));
}

float AIGIShellCasingActor::GetWeatherImpactNoiseMultiplier() const
{
    switch (SpawnWeatherState.WeatherType)
    {
        case EIGIWeatherType::Rain:
            return FMath::Lerp(0.80f, 0.60f, SpawnWeatherState.SurfaceWetness);

        case EIGIWeatherType::Snow:
            return FMath::Lerp(0.60f, 0.30f, SpawnWeatherState.SnowDepthFactor);

        case EIGIWeatherType::Storm:
            return 0.72f;

        case EIGIWeatherType::Sandstorm:
            return 0.62f;

        case EIGIWeatherType::Clear:
        default:
            return 1.0f;
    }
}

float AIGIShellCasingActor::GetSurfaceImpactNoiseMultiplier(
    const EPhysicalSurface SurfaceType)
{
    switch (SurfaceType)
    {
        case SurfaceType2: // Mud
            return 0.35f;

        case SurfaceType3: // Snow
            return 0.30f;

        case SurfaceType4: // Sand
            return 0.40f;

        case SurfaceType5: // Grass
            return 0.45f;

        case SurfaceType6: // Concrete
            return 1.15f;

        case SurfaceType7: // Metal
            return 1.55f;

        case SurfaceType8: // Water
            return 0.55f;

        case SurfaceType1: // Dirt
            return 0.70f;

        default:
            return 1.0f;
    }
}
