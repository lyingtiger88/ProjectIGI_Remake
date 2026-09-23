#include "Weapons/IGIWeaponBase.h"

#include "Acoustics/BDFRAcousticEventLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapons/IGIWeaponAttachmentComponent.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIWeaponBase::AIGIWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AttachmentComponent = CreateDefaultSubobject<UIGIWeaponAttachmentComponent>(TEXT("AttachmentComponent"));
}

void AIGIWeaponBase::InitializeFromData(UIGIWeaponDataAsset* InWeaponData)
{
    WeaponData = InWeaponData;

    if (!IsValid(WeaponData) || WeaponData->WeaponMesh.IsNull())
    {
        return;
    }

    if (USkeletalMesh* Mesh = WeaponData->WeaponMesh.LoadSynchronous(); IsValid(Mesh))
    {
        WeaponMesh->SetSkeletalMeshAsset(Mesh);
    }
}

void AIGIWeaponBase::EquipTo(
    AActor* NewOwner,
    USceneComponent* Parent,
    const FName SocketName)
{
    SetOwner(NewOwner);

    if (IsValid(Parent))
    {
        AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    }

    ReportEquipNoise();
}

void AIGIWeaponBase::HolsterTo(
    AActor* NewOwner,
    USceneComponent* Parent,
    const FName SocketName)
{
    SetOwner(NewOwner);

    if (IsValid(Parent))
    {
        AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
    }
}

void AIGIWeaponBase::DropWeapon()
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetOwner(nullptr);
}

bool AIGIWeaponBase::IsSuppressed() const
{
    const bool bIntegral = IsValid(WeaponData) && WeaponData->bIntegralSuppressor;
    const bool bAttached = IsValid(AttachmentComponent) && AttachmentComponent->HasSuppressor();
    return bIntegral || bAttached;
}

float AIGIWeaponBase::GetEffectiveShotHearingRadius() const
{
    if (!IsValid(WeaponData))
    {
        return 0.0f;
    }

    float Multiplier = IsValid(AttachmentComponent)
        ? AttachmentComponent->GetGunshotNoiseMultiplier()
        : 1.0f;

    if (WeaponData->bIntegralSuppressor)
    {
        Multiplier *= WeaponData->IntegralSuppressorNoiseMultiplier;
    }

    return FMath::Max(0.0f, WeaponData->BaseShotHearingRadius * Multiplier);
}

float AIGIWeaponBase::GetCarryNoiseContribution() const
{
    if (!IsValid(WeaponData))
    {
        return 0.0f;
    }

    const float AttachmentMultiplier = IsValid(AttachmentComponent)
        ? AttachmentComponent->GetMovementNoiseMultiplier()
        : 1.0f;

    return FMath::Max(0.0f, WeaponData->CarryNoiseContribution * AttachmentMultiplier);
}

float AIGIWeaponBase::GetTotalWeightKg() const
{
    const float WeaponWeight = IsValid(WeaponData) ? WeaponData->WeightKg : 0.0f;
    const float AttachmentWeight = IsValid(AttachmentComponent)
        ? AttachmentComponent->GetAttachmentWeightKg()
        : 0.0f;

    return FMath::Max(0.0f, WeaponWeight + AttachmentWeight);
}

float AIGIWeaponBase::GetEffectiveRecoil() const
{
    const float Base = IsValid(WeaponData) ? WeaponData->BaseRecoil : 1.0f;
    return Base * (IsValid(AttachmentComponent) ? AttachmentComponent->GetRecoilMultiplier() : 1.0f);
}

float AIGIWeaponBase::GetEffectiveSpread() const
{
    const float Base = IsValid(WeaponData) ? WeaponData->BaseSpread : 1.0f;
    return Base * (IsValid(AttachmentComponent) ? AttachmentComponent->GetSpreadMultiplier() : 1.0f);
}

float AIGIWeaponBase::GetEffectiveMuzzleFlashScale() const
{
    const float Base = IsValid(WeaponData) ? WeaponData->BaseMuzzleFlashScale : 1.0f;
    return Base * (IsValid(AttachmentComponent) ? AttachmentComponent->GetMuzzleFlashMultiplier() : 1.0f);
}

void AIGIWeaponBase::ReportGunshotNoise()
{
    if (!IsValid(WeaponData))
    {
        return;
    }

    UBDFRAcousticEventLibrary::ReportGunshot(
        this,
        GetAcousticInstigator(),
        GetAcousticLocation(),
        IsSuppressed(),
        GetEffectiveShotHearingRadius());
}

void AIGIWeaponBase::ReportEquipNoise()
{
    if (!IsValid(WeaponData))
    {
        return;
    }

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        GetAcousticInstigator(),
        GetActorLocation(),
        WeaponData->EquipNoiseLoudness,
        WeaponData->EquipNoiseRadius,
        TEXT("BDFR.Acoustic.Weapon.Equip"));
}

void AIGIWeaponBase::ReportReloadNoise()
{
    if (!IsValid(WeaponData))
    {
        return;
    }

    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        GetAcousticInstigator(),
        GetActorLocation(),
        WeaponData->ReloadNoiseLoudness,
        WeaponData->ReloadNoiseRadius,
        TEXT("BDFR.Acoustic.Weapon.Reload"));
}

void AIGIWeaponBase::ReportAttachmentHandlingNoise()
{
    UBDFRAcousticEventLibrary::ReportAcousticEvent(
        this,
        GetAcousticInstigator(),
        GetActorLocation(),
        0.10f,
        650.0f,
        TEXT("BDFR.Acoustic.Weapon.Attachment"));
}

FVector AIGIWeaponBase::GetAcousticLocation() const
{
    if (IsValid(WeaponData) && IsValid(WeaponMesh) && WeaponMesh->DoesSocketExist(WeaponData->MuzzleSocket))
    {
        return WeaponMesh->GetSocketLocation(WeaponData->MuzzleSocket);
    }

    return GetActorLocation();
}

AActor* AIGIWeaponBase::GetAcousticInstigator() const
{
    return IsValid(GetOwner()) ? GetOwner() : const_cast<AIGIWeaponBase*>(this);
}
