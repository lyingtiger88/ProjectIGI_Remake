#include "Inventory/IGILootableInventoryComponent.h"

#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIFirearmBase.h"
#include "Weapons/IGIWeaponDataAsset.h"

UIGILootableInventoryComponent::UIGILootableInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 UIGILootableInventoryComponent::LootAllRemainingAmmunition(
    AActor* RecipientActor,
    const bool bIncludeLoadedMagazines)
{
    UIGIInventoryComponent* RecipientInventory =
        IsValid(RecipientActor)
            ? RecipientActor->FindComponentByClass<UIGIInventoryComponent>()
            : nullptr;

    return LootAllRemainingAmmunitionToInventory(
        RecipientInventory,
        bIncludeLoadedMagazines);
}

int32 UIGILootableInventoryComponent::LootAllRemainingAmmunitionToInventory(
    UIGIInventoryComponent* RecipientInventory,
    const bool bIncludeLoadedMagazines)
{
    UIGIInventoryComponent* SourceInventory = GetSourceInventory();

    if (!IsValid(SourceInventory) ||
        !IsValid(RecipientInventory) ||
        SourceInventory == RecipientInventory)
    {
        return 0;
    }

    const int32 BeforeTotal = GetTotalRemainingRounds(bIncludeLoadedMagazines);
    int32 TotalTransferred = 0;

    const TArray<FName> AmmoTypes = SourceInventory->GetAmmoTypes();
    for (const FName AmmoType : AmmoTypes)
    {
        TotalTransferred += SourceInventory->TransferReserveAmmoTo(
            RecipientInventory,
            AmmoType);
    }

    if (!bIncludeLoadedMagazines)
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("IGI loot ammo: source=%s before=%d transferred=%d remaining=%d"),
            *GetNameSafe(GetOwner()),
            BeforeTotal,
            TotalTransferred,
            GetTotalRemainingRounds(false));

        return TotalTransferred;
    }

    const TArray<AIGIWeaponBase*> StoredWeapons = SourceInventory->GetStoredWeapons();
    for (AIGIWeaponBase* Weapon : StoredWeapons)
    {
        AIGIFirearmBase* Firearm = Cast<AIGIFirearmBase>(Weapon);
        const UIGIWeaponDataAsset* WeaponData =
            IsValid(Firearm) ? Firearm->GetWeaponData() : nullptr;

        if (!IsValid(Firearm) ||
            !IsValid(WeaponData) ||
            WeaponData->AmmoType.IsNone())
        {
            continue;
        }

        const int32 LoadedRounds = Firearm->GetCurrentMagazineAmmo();
        if (LoadedRounds <= 0)
        {
            continue;
        }

        const int32 TargetLimit =
            RecipientInventory->GetAmmoCarryLimit(WeaponData->AmmoType);

        if (TargetLimit <= 0)
        {
            continue;
        }

        const int32 Accepted = RecipientInventory->AddAmmo(
            WeaponData->AmmoType,
            LoadedRounds,
            TargetLimit);

        if (Accepted > 0)
        {
            Firearm->ExtractMagazineAmmo(Accepted);
            TotalTransferred += Accepted;
        }
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI loot ammo: source=%s before=%d transferred=%d remaining=%d"),
        *GetNameSafe(GetOwner()),
        BeforeTotal,
        TotalTransferred,
        GetTotalRemainingRounds(true));

    return TotalTransferred;
}

int32 UIGILootableInventoryComponent::GetTotalRemainingRounds(
    const bool bIncludeLoadedMagazines) const
{
    const UIGIInventoryComponent* SourceInventory = GetSourceInventory();
    if (!IsValid(SourceInventory))
    {
        return 0;
    }

    int32 Total = 0;

    for (const FName AmmoType : SourceInventory->GetAmmoTypes())
    {
        Total += SourceInventory->GetAmmoCount(AmmoType);
    }

    if (!bIncludeLoadedMagazines)
    {
        return Total;
    }

    for (AIGIWeaponBase* Weapon : SourceInventory->GetStoredWeapons())
    {
        if (const AIGIFirearmBase* Firearm = Cast<AIGIFirearmBase>(Weapon);
            IsValid(Firearm))
        {
            Total += Firearm->GetCurrentMagazineAmmo();
        }
    }

    return Total;
}

UIGIInventoryComponent* UIGILootableInventoryComponent::GetSourceInventory() const
{
    return GetOwner() != nullptr
        ? GetOwner()->FindComponentByClass<UIGIInventoryComponent>()
        : nullptr;
}
