#include "Weapons/IGIFlareProjectileActor.h"

#include "Acoustics/BDFRAcousticEventLibrary.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Environment/IGIWeatherWorldSubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AIGIFlareProjectileActor::AIGIFlareProjectileActor()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->InitSphereRadius(5.0f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    FlareMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlareMesh"));
    FlareMesh->SetupAttachment(CollisionSphere);
    FlareMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionSphere;
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 2200.0f;
    ProjectileMovement->ProjectileGravityScale = 0.65f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.15f;
    ProjectileMovement->Friction = 0.55f;

    FlareLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FlareLight"));
    FlareLight->SetupAttachment(CollisionSphere);
    FlareLight->SetIntensity(BaseLightIntensity);
    FlareLight->SetAttenuationRadius(LightAttenuationRadius);
    FlareLight->SetCastShadows(true);
    FlareLight->SetVisibility(false);

    ProjectileMovement->OnProjectileStop.AddDynamic(this, &ThisClass::HandleProjectileStop);
}

void AIGIFlareProjectileActor::BeginPlay()
{
    Super::BeginPlay();

    ApplyWeatherToLight();
    ActivateSignal(GetActorLocation());
}

void AIGIFlareProjectileActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!IsValid(ProjectileMovement) || !ProjectileMovement->IsActive())
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
    if (!WindVelocity.IsNearlyZero())
    {
        ProjectileMovement->Velocity +=
            WindVelocity * WindAccelerationScale * DeltaSeconds;
    }
}

void AIGIFlareProjectileActor::InitializeFlare(
    const EIGIFlarePurpose InPurpose,
    const float InitialSpeed,
    const float LifeSeconds,
    UStaticMesh* InMesh,
    UNiagaraSystem* InTrailEffect)
{
    FlarePurpose = InPurpose;

    if (IsValid(FlareMesh) && IsValid(InMesh))
    {
        FlareMesh->SetStaticMesh(InMesh);
    }

    if (IsValid(ProjectileMovement))
    {
        ProjectileMovement->InitialSpeed = FMath::Max(100.0f, InitialSpeed);
        ProjectileMovement->MaxSpeed = FMath::Max(ProjectileMovement->InitialSpeed, InitialSpeed * 1.25f);
        ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
    }

    if (IsValid(InTrailEffect))
    {
        TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            InTrailEffect,
            CollisionSphere,
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true);
    }

    SetLifeSpan(FMath::Max(1.0f, LifeSeconds));
    ApplyWeatherToLight();
    ActivateSignal(GetActorLocation());
}

void AIGIFlareProjectileActor::HandleProjectileStop(const FHitResult& ImpactResult)
{
    if (IsValid(ProjectileMovement))
    {
        ProjectileMovement->Velocity *= ImpactBounceDamping;
    }

    ActivateSignal(ImpactResult.ImpactPoint);
}

void AIGIFlareProjectileActor::ActivateSignal(const FVector& Location)
{
    if (!bActivated)
    {
        bActivated = true;

        if (IsValid(FlareLight))
        {
            FlareLight->SetVisibility(true);
        }
    }

    OnFlareActivated.Broadcast(this, FlarePurpose, Location);

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        GetOwner(),
        Location,
        0.08f,
        500.0f,
        TEXT("BDFR.Acoustic.Weapon.FlareBurn"));
}

void AIGIFlareProjectileActor::ApplyWeatherToLight()
{
    if (!IsValid(FlareLight))
    {
        return;
    }

    float IntensityMultiplier = 1.0f;

    if (UWorld* World = GetWorld(); IsValid(World))
    {
        if (const UIGIWeatherWorldSubsystem* Weather =
                World->GetSubsystem<UIGIWeatherWorldSubsystem>();
            IsValid(Weather))
        {
            const FIGIWeatherState WeatherState = Weather->GetWeatherState();

            switch (WeatherState.WeatherType)
            {
                case EIGIWeatherType::Rain:
                    IntensityMultiplier = FMath::Lerp(1.0f, 0.82f, WeatherState.PrecipitationIntensity);
                    break;

                case EIGIWeatherType::Snow:
                    IntensityMultiplier = 0.90f;
                    break;

                case EIGIWeatherType::Storm:
                    IntensityMultiplier = 0.78f;
                    break;

                case EIGIWeatherType::Sandstorm:
                    IntensityMultiplier = 0.62f;
                    break;

                case EIGIWeatherType::Clear:
                default:
                    IntensityMultiplier = 1.0f;
                    break;
            }
        }
    }

    FlareLight->SetIntensity(BaseLightIntensity * IntensityMultiplier);
    FlareLight->SetAttenuationRadius(LightAttenuationRadius);
}
