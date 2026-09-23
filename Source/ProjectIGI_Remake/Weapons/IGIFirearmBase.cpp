#include "Weapons/IGIFirearmBase.h"

#include "Inventory/IGIInventoryComponent.h"
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
