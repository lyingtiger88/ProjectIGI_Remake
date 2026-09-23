#include "Pickups/IGIWeaponPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIFirearmBase.h"
#include "Weapons/IGIFlareGunBase.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponDataAsset.h"
#include "UObject/Package.h"

AIGIWeaponPickupActor::AIGIWeaponPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->InitSphereRadius(85.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetGenerateOverlapEvents(true);
    PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(PickupSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    WeaponClass = AIGIFirearmBase::StaticClass();
}

void AIGIWeaponPickupActor::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(PickupSphere))
    {
        PickupSphere->SetGenerateOverlapEvents(true);
        PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandlePickupOverlap);
        PickupSphere->UpdateOverlaps();

        TArray<AActor*> OverlappingActors;
        PickupSphere->GetOverlappingActors(OverlappingActors, AIGIPlayerCharacter::StaticClass());

        for (AActor* Actor : OverlappingActors)
        {
            if (TryPickupByActor(Actor))
            {
                return;
            }
        }
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI Weapon Pickup ready: %s | Preset=%d | AutoEquip=%s"),
        *GetName(),
        static_cast<int32>(PrototypePreset),
        bAutoEquip ? TEXT("true") : TEXT("false"));
}

void AIGIWeaponPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    TryPickupByActor(OtherActor);
}

bool AIGIWeaponPickupActor::TryPickupByActor(AActor* OtherActor)
{
    if (!TryGiveWeaponTo(OtherActor))
    {
        return false;
    }

    Destroy();
    return true;
}

bool AIGIWeaponPickupActor::TryGiveWeaponTo(AActor* OtherActor)
{
    AIGIPlayerCharacter* Player = Cast<AIGIPlayerCharacter>(OtherActor);
    UIGIWeaponDataAsset* ResolvedWeaponData = ResolveWeaponData();

    if (!IsValid(Player))
    {
        UE_LOG(
            LogTemp,
            Verbose,
            TEXT("IGI Weapon Pickup '%s' ignored overlap actor '%s' because it is not AIGIPlayerCharacter."),
            *GetName(),
            IsValid(OtherActor) ? *OtherActor->GetName() : TEXT("<invalid>"));
        return false;
    }

    if (!IsValid(ResolvedWeaponData))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("IGI Weapon Pickup '%s' has no valid WeaponData and no active prototype preset."),
            *GetName());
        return false;
    }

    if (ResolvedWeaponData->WeaponId == EIGIWeaponId::FlareGun &&
        (!WeaponClass || WeaponClass == AIGIFirearmBase::StaticClass()))
    {
        WeaponClass = AIGIFlareGunBase::StaticClass();
    }
    else if (!WeaponClass || WeaponClass->HasAnyClassFlags(CLASS_Abstract))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("IGI Weapon Pickup '%s' has an invalid/abstract WeaponClass. Falling back to AIGIFirearmBase."),
            *GetName());
        WeaponClass = AIGIFirearmBase::StaticClass();
    }

    UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();
    UWorld* World = GetWorld();

    if (!IsValid(Inventory) || !IsValid(World))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("IGI Weapon Pickup '%s' could not resolve player inventory or world."),
            *GetName());
        return false;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = Player;
    SpawnParameters.Instigator = Player;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AIGIWeaponBase* Weapon = World->SpawnActor<AIGIWeaponBase>(
        WeaponClass,
        GetActorTransform(),
        SpawnParameters);

    if (!IsValid(Weapon))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("IGI Weapon Pickup '%s' failed to spawn weapon class '%s'."),
            *GetName(),
            *GetNameSafe(WeaponClass.Get()));
        return false;
    }

    Weapon->InitializeFromData(ResolvedWeaponData);

    EIGICarrySlot StoredSlot = EIGICarrySlot::Weapon01;
    if (!Inventory->TryStoreWeapon(Weapon, StoredSlot))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("IGI Weapon Pickup '%s' could not store '%s'; no compatible/free carry slot."),
            *GetName(),
            *ResolvedWeaponData->DisplayName.ToString());
        Weapon->Destroy();
        return false;
    }

    if (!ResolvedWeaponData->AmmoType.IsNone() && InitialReserveAmmo > 0)
    {
        Inventory->AddAmmo(
            ResolvedWeaponData->AmmoType,
            InitialReserveAmmo,
            FMath::Max(InitialReserveAmmo, ResolvedWeaponData->MaxReserveAmmo));
    }

    bool bEquipped = false;

    if (bAutoEquip)
    {
        bEquipped = Inventory->EquipWeaponInSlot(StoredSlot);

        if (!bEquipped)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("IGI picked up '%s' but AutoEquip failed for slot %d."),
                *ResolvedWeaponData->DisplayName.ToString(),
                static_cast<int32>(StoredSlot));
        }
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up weapon '%s' into slot %d. AutoEquip=%s Equipped=%s Visual=%s"),
        *ResolvedWeaponData->DisplayName.ToString(),
        static_cast<int32>(StoredSlot),
        bAutoEquip ? TEXT("true") : TEXT("false"),
        bEquipped ? TEXT("true") : TEXT("false"),
        IsValid(Weapon->GetWeaponVisualComponent()) ? *Weapon->GetWeaponVisualComponent()->GetName() : TEXT("<none>"));

    return true;
}

UIGIWeaponDataAsset* AIGIWeaponPickupActor::ResolveWeaponData()
{
    if (IsValid(WeaponData))
    {
        return WeaponData;
    }

    if (IsValid(RuntimePrototypeData))
    {
        return RuntimePrototypeData;
    }

    if (PrototypePreset == EIGIPrototypeWeaponPreset::Glock17)
    {
        RuntimePrototypeData = CreateGlock17PrototypeData();
    }
    else if (PrototypePreset == EIGIPrototypeWeaponPreset::FlareGun)
    {
        RuntimePrototypeData = CreateFlareGunPrototypeData();
    }

    return RuntimePrototypeData;
}

UIGIWeaponDataAsset* AIGIWeaponPickupActor::CreateGlock17PrototypeData()
{
    UIGIWeaponDataAsset* Data = NewObject<UIGIWeaponDataAsset>(GetTransientPackage());
    if (!IsValid(Data))
    {
        return nullptr;
    }

    Data->WeaponId = EIGIWeaponId::Glock17;
    Data->DisplayName = FText::FromString(TEXT("Glock 17"));
    Data->WeaponMesh = PrototypeWeaponMesh;

    if (PrototypeWeaponMesh.IsNull() && IsValid(PickupMesh) && IsValid(PickupMesh->GetStaticMesh()))
    {
        Data->WeaponStaticMesh = PickupMesh->GetStaticMesh();
    }

    Data->WeaponFamily = EIGIWeaponFamily::Pistol;
    Data->HandlingProfile = EIGIHandlingProfile::Pistol;
    Data->CompatibleCarrySlots = {
        EIGICarrySlot::Weapon03,
        EIGICarrySlot::Weapon01,
        EIGICarrySlot::Weapon02,
        EIGICarrySlot::Weapon04
    };
    Data->WeightKg = 0.92f;
    Data->CarryNoiseContribution = 0.035f;
    Data->SupportedAttachmentSlots = {
        EIGIAttachmentSlot::Muzzle,
        EIGIAttachmentSlot::Optic,
        EIGIAttachmentSlot::SideRail,
        EIGIAttachmentSlot::Magazine
    };
    Data->AmmoType = TEXT("9x19mm");
    Data->MagazineCapacity = 17;
    Data->MaxReserveAmmo = 102;
    Data->SupportedFireModes = {EIGIFireMode::SemiAutomatic};
    Data->DefaultFireMode = EIGIFireMode::SemiAutomatic;
    Data->BaseDamage = 28.0f;
    Data->EffectiveRangeCm = 5000.0f;
    Data->BaseRecoil = 1.0f;
    Data->BaseSpread = 0.35f;
    Data->AimSpeedMultiplier = 1.0f;
    Data->BaseShotHearingRadius = 6500.0f;
    Data->EquipNoiseLoudness = 0.10f;
    Data->EquipNoiseRadius = 650.0f;
    Data->ReloadNoiseLoudness = 0.14f;
    Data->ReloadNoiseRadius = 800.0f;
    Data->BaseMuzzleFlashScale = 1.0f;
    Data->MuzzleFlashEffect = PrototypeMuzzleFlashEffect;
    Data->MuzzleSmokeEffect = PrototypeMuzzleSmokeEffect;
    Data->ShellCasingMesh = PrototypeCasingMesh;

    return Data;
}


UIGIWeaponDataAsset* AIGIWeaponPickupActor::CreateFlareGunPrototypeData()
{
    UIGIWeaponDataAsset* Data = NewObject<UIGIWeaponDataAsset>(GetTransientPackage());
    if (!IsValid(Data))
    {
        return nullptr;
    }

    Data->WeaponId = EIGIWeaponId::FlareGun;
    Data->DisplayName = FText::FromString(TEXT("Flare Gun"));
    Data->WeaponMesh = PrototypeWeaponMesh;

    if (PrototypeWeaponMesh.IsNull() && IsValid(PickupMesh) && IsValid(PickupMesh->GetStaticMesh()))
    {
        Data->WeaponStaticMesh = PickupMesh->GetStaticMesh();
    }

    Data->WeaponFamily = EIGIWeaponFamily::Launcher;
    Data->HandlingProfile = EIGIHandlingProfile::Pistol;
    Data->CompatibleCarrySlots = {
        EIGICarrySlot::Weapon03,
        EIGICarrySlot::Weapon04,
        EIGICarrySlot::Weapon01,
        EIGICarrySlot::Weapon02
    };
    Data->WeightKg = 0.75f;
    Data->CarryNoiseContribution = 0.025f;
    Data->AmmoType = TEXT("Flare");
    Data->MagazineCapacity = 1;
    Data->MaxReserveAmmo = 6;
    Data->SupportedFireModes = {EIGIFireMode::SemiAutomatic};
    Data->DefaultFireMode = EIGIFireMode::SemiAutomatic;
    Data->BaseDamage = 0.0f;
    Data->EffectiveRangeCm = 12000.0f;
    Data->BaseRecoil = 1.30f;
    Data->BaseSpread = 0.20f;
    Data->AimSpeedMultiplier = 0.95f;
    Data->BaseShotHearingRadius = 3500.0f;
    Data->EquipNoiseLoudness = 0.08f;
    Data->EquipNoiseRadius = 500.0f;
    Data->ReloadNoiseLoudness = 0.12f;
    Data->ReloadNoiseRadius = 650.0f;
    Data->BaseMuzzleFlashScale = 1.25f;
    Data->MuzzleFlashEffect = PrototypeMuzzleFlashEffect;
    Data->MuzzleSmokeEffect = PrototypeMuzzleSmokeEffect;
    Data->MuzzleSmokeScale = 1.30f;
    Data->FlarePurpose = PrototypeFlarePurpose;
    Data->FlareProjectileMesh = PrototypeFlareProjectileMesh;
    Data->FlareTrailEffect = PrototypeFlareTrailEffect;
    Data->FlareLaunchSpeed = 1800.0f;
    Data->FlareLifeSeconds = 22.0f;

    return Data;
}
