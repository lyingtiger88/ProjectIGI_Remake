#include "Weapons/IGIFlareGunBase.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "NiagaraSystem.h"
#include "Weapons/IGIFlareProjectileActor.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIFlareGunBase::AIGIFlareGunBase()
{
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

    const TSubclassOf<AIGIFlareProjectileActor> ProjectileClass =
        WeaponData->FlareProjectileClass
            ? WeaponData->FlareProjectileClass
            : AIGIFlareProjectileActor::StaticClass();

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = GetOwner();
    SpawnParameters.Instigator = GetInstigator();
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AIGIFlareProjectileActor* Flare = World->SpawnActor<AIGIFlareProjectileActor>(
        ProjectileClass,
        MuzzleTransform.GetLocation(),
        ViewRotation,
        SpawnParameters);

    if (!IsValid(Flare))
    {
        return false;
    }

    UStaticMesh* ProjectileMesh =
        WeaponData->FlareProjectileMesh.IsNull()
            ? nullptr
            : WeaponData->FlareProjectileMesh.LoadSynchronous();

    UNiagaraSystem* TrailEffect =
        WeaponData->FlareTrailEffect.IsNull()
            ? nullptr
            : WeaponData->FlareTrailEffect.LoadSynchronous();

    Flare->InitializeFlare(
        WeaponData->FlarePurpose,
        WeaponData->FlareLaunchSpeed,
        WeaponData->FlareLifeSeconds,
        ProjectileMesh,
        TrailEffect);

    LastShotImpactLocation =
        MuzzleTransform.GetLocation() +
        ViewRotation.Vector() * WeaponData->EffectiveRangeCm;

    return true;
}
