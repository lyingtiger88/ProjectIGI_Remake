#include "Weapons/IGIFlareGunBase.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "NiagaraSystem.h"
#include "Weapons/IGIFlareProjectileActor.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIFlareGunBase::AIGIFlareGunBase()
{
    FlareProjectileClass = AIGIFlareProjectileActor::StaticClass();
}

bool AIGIFlareGunBase::FireHitscan(AController* InstigatorController)
{
    bLastShotHit = false;
    LastShotImpactLocation = FVector::ZeroVector;

    if (!IsValid(WeaponData) ||
        !IsValid(InstigatorController) ||
        !NotifyShotFired())
    {
        return false;
    }

    PlayShotEffects();

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return false;
    }

    FVector ViewLocation = GetActorLocation();
    FRotator ViewRotation = GetActorRotation();
    InstigatorController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FTransform MuzzleTransform =
        GetWeaponSocketTransform(WeaponData->MuzzleSocket);

    const FVector SpawnLocation = MuzzleTransform.GetLocation();
    const FRotator SpawnRotation = ViewRotation;

    const TSubclassOf<AIGIFlareProjectileActor> ProjectileClass =
        FlareProjectileClass
            ? FlareProjectileClass
            : AIGIFlareProjectileActor::StaticClass();

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = GetOwner();
    SpawnParameters.Instigator = GetInstigator();
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AIGIFlareProjectileActor* Flare = World->SpawnActor<AIGIFlareProjectileActor>(
        ProjectileClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParameters);

    if (!IsValid(Flare))
    {
        return false;
    }

    UStaticMesh* ProjectileMesh =
        FlareProjectileMesh.IsNull()
            ? nullptr
            : FlareProjectileMesh.LoadSynchronous();

    UNiagaraSystem* TrailEffect =
        FlareTrailEffect.IsNull()
            ? nullptr
            : FlareTrailEffect.LoadSynchronous();

    Flare->InitializeFlare(
        FlarePurpose,
        FlareLaunchSpeed,
        FlareLifeSeconds,
        ProjectileMesh,
        TrailEffect);

    LastShotImpactLocation =
        SpawnLocation + SpawnRotation.Vector() * WeaponData->EffectiveRangeCm;

    return true;
}
