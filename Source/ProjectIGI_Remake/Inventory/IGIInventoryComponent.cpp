#include "Inventory/IGIInventoryComponent.h"

#include "Combat/IGICombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponDataAsset.h"

UIGIInventoryComponent::UIGIInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGIInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    CombatComponent = GetOwner() != nullptr
        ? GetOwner()->FindComponentByClass<UIGICombatComponent>()
        : nullptr;
}

bool UIGIInventoryComponent::TryStoreWeapon(AIGIWeaponBase* Weapon, EIGICarrySlot& OutSlot)
{
    if (!IsValid(Weapon))
    {
        return false;
    }

    for (const TPair<EIGICarrySlot, TObjectPtr<AIGIWeaponBase>>& Pair : StoredWeapons)
    {
        if (Pair.Value == Weapon)
        {
            OutSlot = Pair.Key;
            return true;
        }
    }

    if (!FindFreeCompatibleSlot(Weapon, OutSlot))
    {
        return false;
    }

    StoredWeapons.Add(OutSlot, Weapon);
    AttachStoredWeapon(Weapon, OutSlot);
    return true;
}

bool UIGIInventoryComponent::RemoveWeapon(AIGIWeaponBase* Weapon)
{
    if (!IsValid(Weapon))
    {
        return false;
    }

    EIGICarrySlot SlotToRemove = EIGICarrySlot::Weapon01;
    bool bFound = false;

    for (const TPair<EIGICarrySlot, TObjectPtr<AIGIWeaponBase>>& Pair : StoredWeapons)
    {
        if (Pair.Value == Weapon)
        {
            SlotToRemove = Pair.Key;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        return false;
    }

    if (bHasActiveWeaponSlot && SlotToRemove == ActiveWeaponSlot)
    {
        bHasActiveWeaponSlot = false;

        if (IsValid(CombatComponent))
        {
            CombatComponent->SetActiveWeapon(nullptr);
        }
    }

    StoredWeapons.Remove(SlotToRemove);
    Weapon->DropWeapon();
    return true;
}

bool UIGIInventoryComponent::EquipWeaponInSlot(const EIGICarrySlot Slot)
{
    if (!IsPhysicalWeaponSlot(Slot))
    {
        return false;
    }

    AIGIWeaponBase* Weapon = GetWeaponInSlot(Slot);
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (!IsValid(Weapon) || !IsValid(Character) || !IsValid(Character->GetMesh()))
    {
        return false;
    }

    if (bHasActiveWeaponSlot && ActiveWeaponSlot == Slot)
    {
        return true;
    }

    if (bHasActiveWeaponSlot)
    {
        if (AIGIWeaponBase* PreviousWeapon = GetWeaponInSlot(ActiveWeaponSlot); IsValid(PreviousWeapon))
        {
            AttachStoredWeapon(PreviousWeapon, ActiveWeaponSlot);
        }
    }

    const UIGIWeaponDataAsset* Data = Weapon->GetWeaponData();
    const FName EquipSocket = IsValid(Data) && !Data->EquippedSocket.IsNone()
        ? Data->EquippedSocket
        : FName(TEXT("SCK_Weapon_Hand_R"));

    Weapon->EquipTo(Character, Character->GetMesh(), EquipSocket);

    ActiveWeaponSlot = Slot;
    bHasActiveWeaponSlot = true;

    if (!IsValid(CombatComponent))
    {
        CombatComponent = GetOwner()->FindComponentByClass<UIGICombatComponent>();
    }

    if (IsValid(CombatComponent))
    {
        CombatComponent->SetActiveWeapon(Weapon);
    }

    return true;
}

bool UIGIInventoryComponent::UnequipActiveWeapon()
{
    if (!bHasActiveWeaponSlot)
    {
        return false;
    }

    if (AIGIWeaponBase* Weapon = GetWeaponInSlot(ActiveWeaponSlot); IsValid(Weapon))
    {
        AttachStoredWeapon(Weapon, ActiveWeaponSlot);
    }

    bHasActiveWeaponSlot = false;

    if (IsValid(CombatComponent))
    {
        CombatComponent->SetActiveWeapon(nullptr);
    }

    return true;
}

AIGIWeaponBase* UIGIInventoryComponent::GetWeaponInSlot(const EIGICarrySlot Slot) const
{
    const TObjectPtr<AIGIWeaponBase>* Found = StoredWeapons.Find(Slot);
    return Found != nullptr ? Found->Get() : nullptr;
}

AIGIWeaponBase* UIGIInventoryComponent::GetActiveWeapon() const
{
    return bHasActiveWeaponSlot ? GetWeaponInSlot(ActiveWeaponSlot) : nullptr;
}

FName UIGIInventoryComponent::GetSocketNameForSlot(const EIGICarrySlot Slot) const
{
    switch (Slot)
    {
        case EIGICarrySlot::Weapon01: return TEXT("SCK_Weapon_Back_01");
        case EIGICarrySlot::Weapon02: return TEXT("SCK_Weapon_Back_02");
        case EIGICarrySlot::Weapon03: return TEXT("SCK_Weapon_Hip_R");
        case EIGICarrySlot::Weapon04: return TEXT("SCK_Weapon_Special");
        case EIGICarrySlot::Knife: return TEXT("SCK_Knife");
        case EIGICarrySlot::FragGrenade: return TEXT("SCK_Grenade_Frag");
        case EIGICarrySlot::Flashbang: return TEXT("SCK_Grenade_Flash");
        case EIGICarrySlot::SmokeGrenade: return TEXT("SCK_Grenade_Smoke");
        case EIGICarrySlot::UtilityExplosive: return TEXT("SCK_Utility_Explosive");
        default: return NAME_None;
    }
}

int32 UIGIInventoryComponent::AddAmmo(const FName AmmoType, const int32 Amount, const int32 MaxCarry)
{
    if (AmmoType.IsNone() || Amount <= 0 || MaxCarry <= 0)
    {
        return 0;
    }

    int32& Current = AmmoPools.FindOrAdd(AmmoType);
    const int32 Previous = Current;
    Current = FMath::Clamp(Current + Amount, 0, MaxCarry);
    return Current - Previous;
}

int32 UIGIInventoryComponent::ConsumeAmmo(const FName AmmoType, const int32 Amount)
{
    if (AmmoType.IsNone() || Amount <= 0)
    {
        return 0;
    }

    int32* Current = AmmoPools.Find(AmmoType);
    if (Current == nullptr || *Current <= 0)
    {
        return 0;
    }

    const int32 Consumed = FMath::Min(*Current, Amount);
    *Current -= Consumed;

    if (*Current <= 0)
    {
        AmmoPools.Remove(AmmoType);
    }

    return Consumed;
}

int32 UIGIInventoryComponent::GetAmmoCount(const FName AmmoType) const
{
    const int32* Found = AmmoPools.Find(AmmoType);
    return Found != nullptr ? *Found : 0;
}

int32 UIGIInventoryComponent::AddEquipment(const EIGIEquipmentType EquipmentType, const int32 Amount)
{
    if (Amount <= 0)
    {
        return 0;
    }

    int32& Current = EquipmentCounts.FindOrAdd(EquipmentType);
    const int32 Previous = Current;

    if (EquipmentType == EIGIEquipmentType::C4 ||
        EquipmentType == EIGIEquipmentType::ProximityMine ||
        EquipmentType == EIGIEquipmentType::Claymore)
    {
        const int32 OtherUtilityCount = GetUtilityExplosiveTotal() - Current;
        const int32 RemainingCapacity = FMath::Max(0, MaxUtilityExplosives - OtherUtilityCount);
        Current = FMath::Clamp(Current + Amount, 0, RemainingCapacity);
    }
    else
    {
        Current = FMath::Clamp(Current + Amount, 0, GetEquipmentLimit(EquipmentType));
    }

    return Current - Previous;
}

int32 UIGIInventoryComponent::ConsumeEquipment(const EIGIEquipmentType EquipmentType, const int32 Amount)
{
    if (Amount <= 0)
    {
        return 0;
    }

    int32* Current = EquipmentCounts.Find(EquipmentType);
    if (Current == nullptr || *Current <= 0)
    {
        return 0;
    }

    const int32 Consumed = FMath::Min(*Current, Amount);
    *Current -= Consumed;

    if (*Current <= 0)
    {
        EquipmentCounts.Remove(EquipmentType);
    }

    return Consumed;
}

int32 UIGIInventoryComponent::GetEquipmentCount(const EIGIEquipmentType EquipmentType) const
{
    const int32* Found = EquipmentCounts.Find(EquipmentType);
    return Found != nullptr ? *Found : 0;
}

float UIGIInventoryComponent::GetTotalCarryWeightKg() const
{
    float Total = 0.0f;

    for (const TPair<EIGICarrySlot, TObjectPtr<AIGIWeaponBase>>& Pair : StoredWeapons)
    {
        if (IsValid(Pair.Value))
        {
            Total += Pair.Value->GetTotalWeightKg();
        }
    }

    Total += GetEquipmentCount(EIGIEquipmentType::FragGrenade) * 0.40f;
    Total += GetEquipmentCount(EIGIEquipmentType::Flashbang) * 0.35f;
    Total += GetEquipmentCount(EIGIEquipmentType::SmokeGrenade) * 0.55f;
    Total += GetEquipmentCount(EIGIEquipmentType::C4) * 0.70f;
    Total += GetEquipmentCount(EIGIEquipmentType::ProximityMine) * 1.20f;
    Total += GetEquipmentCount(EIGIEquipmentType::Claymore) * 1.60f;

    return Total;
}

float UIGIInventoryComponent::GetMovementNoiseMultiplier() const
{
    float NoiseContribution = 0.0f;

    for (const TPair<EIGICarrySlot, TObjectPtr<AIGIWeaponBase>>& Pair : StoredWeapons)
    {
        if (IsValid(Pair.Value))
        {
            NoiseContribution += Pair.Value->GetCarryNoiseContribution();
        }
    }

    const int32 LooseEquipmentCount =
        GetEquipmentCount(EIGIEquipmentType::FragGrenade) +
        GetEquipmentCount(EIGIEquipmentType::Flashbang) +
        GetEquipmentCount(EIGIEquipmentType::SmokeGrenade) +
        GetEquipmentCount(EIGIEquipmentType::C4) +
        GetEquipmentCount(EIGIEquipmentType::ProximityMine) +
        GetEquipmentCount(EIGIEquipmentType::Claymore);

    NoiseContribution += LooseEquipmentCount * 0.015f;

    const float ExcessWeight = FMath::Max(0.0f, GetTotalCarryWeightKg() - FreeWeightThresholdKg);
    NoiseContribution += ExcessWeight * NoisePerKgAboveThreshold;

    return FMath::Clamp(1.0f + NoiseContribution, 1.0f, 2.5f);
}

bool UIGIInventoryComponent::FindFreeCompatibleSlot(
    const AIGIWeaponBase* Weapon,
    EIGICarrySlot& OutSlot) const
{
    if (!IsValid(Weapon))
    {
        return false;
    }

    const UIGIWeaponDataAsset* Data = Weapon->GetWeaponData();

    if (IsValid(Data) &&
        (Data->WeaponFamily == EIGIWeaponFamily::MountedWeapon ||
         Data->WeaponFamily == EIGIWeaponFamily::Throwable ||
         Data->WeaponFamily == EIGIWeaponFamily::DeployableExplosive))
    {
        return false;
    }

    TArray<EIGICarrySlot> CandidateSlots;
    if (IsValid(Data) && !Data->CompatibleCarrySlots.IsEmpty())
    {
        CandidateSlots = Data->CompatibleCarrySlots;
    }
    else if (IsValid(Data) && Data->WeaponFamily == EIGIWeaponFamily::Knife)
    {
        CandidateSlots.Add(EIGICarrySlot::Knife);
    }
    else
    {
        CandidateSlots = {
            EIGICarrySlot::Weapon01,
            EIGICarrySlot::Weapon02,
            EIGICarrySlot::Weapon03,
            EIGICarrySlot::Weapon04
        };
    }

    for (const EIGICarrySlot Slot : CandidateSlots)
    {
        if (!IsPhysicalWeaponSlot(Slot) || StoredWeapons.Contains(Slot))
        {
            continue;
        }

        OutSlot = Slot;
        return true;
    }

    return false;
}

bool UIGIInventoryComponent::IsPhysicalWeaponSlot(const EIGICarrySlot Slot) const
{
    return Slot == EIGICarrySlot::Weapon01 ||
        Slot == EIGICarrySlot::Weapon02 ||
        Slot == EIGICarrySlot::Weapon03 ||
        Slot == EIGICarrySlot::Weapon04 ||
        Slot == EIGICarrySlot::Knife;
}

int32 UIGIInventoryComponent::GetEquipmentLimit(const EIGIEquipmentType EquipmentType) const
{
    switch (EquipmentType)
    {
        case EIGIEquipmentType::FragGrenade: return MaxFragGrenades;
        case EIGIEquipmentType::Flashbang: return MaxFlashbangs;
        case EIGIEquipmentType::SmokeGrenade: return MaxSmokeGrenades;
        case EIGIEquipmentType::C4:
        case EIGIEquipmentType::ProximityMine:
        case EIGIEquipmentType::Claymore:
            return MaxUtilityExplosives;
        default:
            return 0;
    }
}

int32 UIGIInventoryComponent::GetUtilityExplosiveTotal() const
{
    return GetEquipmentCount(EIGIEquipmentType::C4) +
        GetEquipmentCount(EIGIEquipmentType::ProximityMine) +
        GetEquipmentCount(EIGIEquipmentType::Claymore);
}

void UIGIInventoryComponent::AttachStoredWeapon(
    AIGIWeaponBase* Weapon,
    const EIGICarrySlot Slot) const
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());

    if (!IsValid(Character) || !IsValid(Character->GetMesh()) || !IsValid(Weapon))
    {
        return;
    }

    Weapon->HolsterTo(Character, Character->GetMesh(), GetSocketNameForSlot(Slot));
}
