#include "Pickups/IGIWeaponPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIFirearmBase.h"
#include "Weapons/IGIWeaponBase.h"
#include "Weapons/IGIWeaponDataAsset.h"

AIGIWeaponPickupActor::AIGIWeaponPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->InitSphereRadius(85.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
        PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandlePickupOverlap);
    }
}

void AIGIWeaponPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (TryGiveWeaponTo(OtherActor))
    {
        Destroy();
    }
}

bool AIGIWeaponPickupActor::TryGiveWeaponTo(AActor* OtherActor)
{
    AIGIPlayerCharacter* Player = Cast<AIGIPlayerCharacter>(OtherActor);
    UIGIWeaponDataAsset* ResolvedWeaponData = ResolveWeaponData();

    if (!IsValid(Player) || !IsValid(ResolvedWeaponData) || !WeaponClass)
    {
        return false;
    }

    UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();
    UWorld* World = GetWorld();

    if (!IsValid(Inventory) || !IsValid(World))
    {
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
        return false;
    }

    Weapon->InitializeFromData(ResolvedWeaponData);

    EIGICarrySlot StoredSlot = EIGICarrySlot::Weapon01;
    if (!Inventory->TryStoreWeapon(Weapon, StoredSlot))
    {
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

    if (bAutoEquip)
    {
        Inventory->EquipWeaponInSlot(StoredSlot);
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up weapon '%s' into slot %d."),
        *ResolvedWeaponData->DisplayName.ToString(),
        static_cast<int32>(StoredSlot));

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

    return RuntimePrototypeData;
}

UIGIWeaponDataAsset* AIGIWeaponPickupActor::CreateGlock17PrototypeData()
{
    UIGIWeaponDataAsset* Data = NewObject<UIGIWeaponDataAsset>(this, TEXT("Runtime_Glock17_Data"));
    if (!IsValid(Data))
    {
        return nullptr;
    }

    Data->WeaponId = EIGIWeaponId::Glock17;
    Data->DisplayName = FText::FromString(TEXT("Glock 17"));
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

    return Data;
}
