#include "Weapons/IGIFirearmBase.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/IGIWeaponAttachmentComponent.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIFirearmBase::AIGIFirearmBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AIGIFirearmBase::BeginPlay()
{
    Super::BeginPlay();
    ResetRuntimeWeaponState();
}

void AIGIFirearmBase::InitializeFromData(UIGIWeaponDataAsset* InWeaponData)
{
    Super::InitializeFromData(InWeaponData);
    ResetRuntimeWeaponState();
}

int32 AIGIFirearmBase::GetMagazineCapacity() const
{
    if (!IsValid(WeaponData))
    {
        return 0;
    }

    const int32 AttachmentBonus = IsValid(AttachmentComponent)
        ? AttachmentComponent->GetMagazineCapacityBonus()
        : 0;

    return FMath::Max(0, WeaponData->MagazineCapacity + AttachmentBonus);
}

bool AIGIFirearmBase::SetFireMode(const EIGIFireMode NewFireMode)
{
    if (!IsValid(WeaponData) || !WeaponData->SupportedFireModes.Contains(NewFireMode))
    {
        return false;
    }

    CurrentFireMode = NewFireMode;
    return true;
}

bool AIGIFirearmBase::TryConsumeRound()
{
    if (CurrentMagazineAmmo <= 0)
    {
        return false;
    }

    --CurrentMagazineAmmo;
    return true;
}

int32 AIGIFirearmBase::ReloadFromInventory(UIGIInventoryComponent* Inventory)
{
    if (!IsValid(Inventory) || !IsValid(WeaponData) || WeaponData->AmmoType.IsNone())
    {
        return 0;
    }

    const int32 Capacity = GetMagazineCapacity();
    const int32 Needed = FMath::Max(0, Capacity - CurrentMagazineAmmo);

    if (Needed <= 0)
    {
        return 0;
    }

    const int32 Consumed = Inventory->ConsumeAmmo(WeaponData->AmmoType, Needed);
    CurrentMagazineAmmo += Consumed;

    if (Consumed > 0)
    {
        ReportReloadNoise();
    }

    return Consumed;
}

bool AIGIFirearmBase::NotifyShotFired()
{
    if (!TryConsumeRound())
    {
        return false;
    }

    ReportGunshotNoise();
    return true;
}

bool AIGIFirearmBase::FireHitscan(AController* InstigatorController)
{
    bLastShotHit = false;
    LastShotImpactLocation = FVector::ZeroVector;

    if (!IsValid(WeaponData) || !IsValid(InstigatorController) || !NotifyShotFired())
    {
        return false;
    }

    FVector ViewLocation = GetActorLocation();
    FRotator ViewRotation = GetActorRotation();
    InstigatorController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const float SpreadRadians = FMath::DegreesToRadians(FMath::Max(0.0f, GetEffectiveSpread()));
    const FVector ShotDirection = SpreadRadians > KINDA_SMALL_NUMBER
        ? FMath::VRandCone(ViewRotation.Vector(), SpreadRadians)
        : ViewRotation.Vector();

    const FVector TraceEnd = ViewLocation + ShotDirection * FMath::Max(1.0f, WeaponData->EffectiveRangeCm);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(IGIWeaponHitscan), true, GetOwner());
    QueryParams.AddIgnoredActor(this);

    FHitResult Hit;
    UWorld* World = GetWorld();

    if (!IsValid(World) || !World->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, WeaponTraceChannel, QueryParams))
    {
        LastShotImpactLocation = TraceEnd;
        return true;
    }

    bLastShotHit = true;
    LastShotImpactLocation = Hit.ImpactPoint;

    AActor* HitActor = Hit.GetActor();
    if (IsValid(HitActor))
    {
        UGameplayStatics::ApplyPointDamage(
            HitActor,
            WeaponData->BaseDamage,
            ShotDirection,
            Hit,
            InstigatorController,
            GetAcousticInstigator(),
            nullptr);
    }

    return true;
}

void AIGIFirearmBase::ResetRuntimeWeaponState()
{
    if (!IsValid(WeaponData))
    {
        CurrentMagazineAmmo = 0;
        CurrentFireMode = EIGIFireMode::SemiAutomatic;
        return;
    }

    CurrentFireMode = WeaponData->SupportedFireModes.Contains(WeaponData->DefaultFireMode)
        ? WeaponData->DefaultFireMode
        : (WeaponData->SupportedFireModes.IsEmpty()
            ? EIGIFireMode::SemiAutomatic
            : WeaponData->SupportedFireModes[0]);

    CurrentMagazineAmmo = bStartLoaded ? GetMagazineCapacity() : 0;
}
