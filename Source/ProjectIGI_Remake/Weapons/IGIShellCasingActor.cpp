#include "Weapons/IGIShellCasingActor.h"

#include "Components/StaticMeshComponent.h"
#include "Environment/IGIWeatherWorldSubsystem.h"
#include "Engine/World.h"

AIGIShellCasingActor::AIGIShellCasingActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
    SetRootComponent(CasingMesh);

    CasingMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    CasingMesh->SetGenerateOverlapEvents(false);
    CasingMesh->SetSimulatePhysics(true);
    CasingMesh->SetEnableGravity(true);
    CasingMesh->SetLinearDamping(0.08f);
    CasingMesh->SetAngularDamping(0.12f);
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
