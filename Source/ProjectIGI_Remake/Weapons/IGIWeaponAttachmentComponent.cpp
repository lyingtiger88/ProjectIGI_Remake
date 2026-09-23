#include "Weapons/IGIWeaponAttachmentComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Weapons/IGIWeaponAttachmentDataAsset.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponDataAsset.h"

UIGIWeaponAttachmentComponent::UIGIWeaponAttachmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

AIGIWeaponBase* UIGIWeaponAttachmentComponent::GetOwnerWeapon() const
{
    return Cast<AIGIWeaponBase>(GetOwner());
}

bool UIGIWeaponAttachmentComponent::CanInstallAttachment(const UIGIWeaponAttachmentDataAsset* Attachment) const
{
    const AIGIWeaponBase* Weapon = GetOwnerWeapon();
    const UIGIWeaponDataAsset* WeaponData = IsValid(Weapon) ? Weapon->GetWeaponData() : nullptr;

    if (!IsValid(Attachment) || !IsValid(WeaponData))
    {
        return false;
    }

    if (!WeaponData->SupportedAttachmentSlots.Contains(Attachment->AttachmentSlot))
    {
        return false;
    }

    const bool bHasExplicitCompatibility =
        !Attachment->CompatibleWeaponIds.IsEmpty() ||
        !Attachment->CompatibleWeaponFamilies.IsEmpty();

    if (!bHasExplicitCompatibility)
    {
        return Attachment->bUniversalWhenCompatibilityEmpty;
    }

    return Attachment->CompatibleWeaponIds.Contains(WeaponData->WeaponId) ||
        Attachment->CompatibleWeaponFamilies.Contains(WeaponData->WeaponFamily);
}

bool UIGIWeaponAttachmentComponent::InstallAttachment(
    UIGIWeaponAttachmentDataAsset* Attachment,
    const bool bReplaceExisting)
{
    if (!CanInstallAttachment(Attachment))
    {
        return false;
    }

    const EIGIAttachmentSlot Slot = Attachment->AttachmentSlot;

    if (InstalledAttachments.Contains(Slot))
    {
        if (!bReplaceExisting)
        {
            return false;
        }

        RemoveAttachment(Slot);
    }

    InstalledAttachments.Add(Slot, Attachment);
    RefreshVisual(Slot);
    OnAttachmentChanged.Broadcast(Slot, Attachment);

    if (AIGIWeaponBase* Weapon = GetOwnerWeapon(); IsValid(Weapon))
    {
        Weapon->ReportAttachmentHandlingNoise();
    }

    return true;
}

bool UIGIWeaponAttachmentComponent::RemoveAttachment(const EIGIAttachmentSlot Slot)
{
    if (!InstalledAttachments.Contains(Slot))
    {
        return false;
    }

    InstalledAttachments.Remove(Slot);
    DestroyVisual(Slot);
    OnAttachmentChanged.Broadcast(Slot, nullptr);

    if (AIGIWeaponBase* Weapon = GetOwnerWeapon(); IsValid(Weapon))
    {
        Weapon->ReportAttachmentHandlingNoise();
    }

    return true;
}

UIGIWeaponAttachmentDataAsset* UIGIWeaponAttachmentComponent::GetAttachment(const EIGIAttachmentSlot Slot) const
{
    const TObjectPtr<UIGIWeaponAttachmentDataAsset>* Found = InstalledAttachments.Find(Slot);
    return Found != nullptr ? Found->Get() : nullptr;
}

bool UIGIWeaponAttachmentComponent::HasSuppressor() const
{
    for (const TPair<EIGIAttachmentSlot, TObjectPtr<UIGIWeaponAttachmentDataAsset>>& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value) && Pair.Value->bSuppressesWeapon)
        {
            return true;
        }
    }

    return false;
}

float UIGIWeaponAttachmentComponent::GetRecoilMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->RecoilMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetSpreadMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->SpreadMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetAimSpeedMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->AimSpeedMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetReloadTimeMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->ReloadTimeMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetGunshotNoiseMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->GunshotNoiseMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetMovementNoiseMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->MovementNoiseMultiplier; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetMuzzleFlashMultiplier() const
{
    float Result = 1.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result *= Pair.Value->MuzzleFlashMultiplier; }
    }
    return Result;
}

int32 UIGIWeaponAttachmentComponent::GetMagazineCapacityBonus() const
{
    int32 Result = 0;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result += Pair.Value->MagazineCapacityBonus; }
    }
    return Result;
}

float UIGIWeaponAttachmentComponent::GetAttachmentWeightKg() const
{
    float Result = 0.0f;
    for (const auto& Pair : InstalledAttachments)
    {
        if (IsValid(Pair.Value)) { Result += Pair.Value->WeightKg; }
    }
    return Result;
}

void UIGIWeaponAttachmentComponent::RefreshVisual(const EIGIAttachmentSlot Slot)
{
    DestroyVisual(Slot);

    AIGIWeaponBase* Weapon = GetOwnerWeapon();
    UIGIWeaponAttachmentDataAsset* Attachment = GetAttachment(Slot);

    if (!IsValid(Weapon) || !IsValid(Attachment) || Attachment->Mesh.IsNull())
    {
        return;
    }

    UStaticMesh* Mesh = Attachment->Mesh.LoadSynchronous();
    USceneComponent* AttachmentParent = Weapon->GetWeaponVisualComponent();

    if (!IsValid(Mesh) || !IsValid(AttachmentParent))
    {
        return;
    }

    UStaticMeshComponent* Visual = NewObject<UStaticMeshComponent>(Weapon);
    if (!IsValid(Visual))
    {
        return;
    }

    Visual->SetStaticMesh(Mesh);
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Weapon->AddInstanceComponent(Visual);
    Visual->RegisterComponent();

    const FName SocketName = Attachment->AttachSocketName.IsNone()
        ? GetDefaultSocketName(Slot)
        : Attachment->AttachSocketName;

    Visual->AttachToComponent(
        AttachmentParent,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SocketName);

    VisualComponents.Add(Slot, Visual);
}

void UIGIWeaponAttachmentComponent::DestroyVisual(const EIGIAttachmentSlot Slot)
{
    TObjectPtr<UStaticMeshComponent>* Found = VisualComponents.Find(Slot);
    if (Found != nullptr && IsValid(Found->Get()))
    {
        Found->Get()->DestroyComponent();
    }

    VisualComponents.Remove(Slot);
}

FName UIGIWeaponAttachmentComponent::GetDefaultSocketName(const EIGIAttachmentSlot Slot)
{
    switch (Slot)
    {
        case EIGIAttachmentSlot::Muzzle: return TEXT("SCK_Attachment_Muzzle");
        case EIGIAttachmentSlot::Optic: return TEXT("SCK_Attachment_Optic");
        case EIGIAttachmentSlot::Underbarrel: return TEXT("SCK_Attachment_Underbarrel");
        case EIGIAttachmentSlot::SideRail: return TEXT("SCK_Attachment_SideRail");
        case EIGIAttachmentSlot::Magazine: return TEXT("SCK_Attachment_Magazine");
        case EIGIAttachmentSlot::Stock: return TEXT("SCK_Attachment_Stock");
        case EIGIAttachmentSlot::Barrel: return TEXT("SCK_Attachment_Barrel");
        case EIGIAttachmentSlot::Special: return TEXT("SCK_Attachment_Special");
        default: return NAME_None;
    }
}
