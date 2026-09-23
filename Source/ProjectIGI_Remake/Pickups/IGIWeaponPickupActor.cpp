#include "Pickups/IGIWeaponPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
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
    if (!IsValid(Player) || !IsValid(WeaponData) || !WeaponClass)
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

    Weapon->InitializeFromData(WeaponData);

    EIGICarrySlot StoredSlot = EIGICarrySlot::Weapon01;
    if (!Inventory->TryStoreWeapon(Weapon, StoredSlot))
    {
        Weapon->Destroy();
        return false;
    }

    if (!WeaponData->AmmoType.IsNone() && InitialReserveAmmo > 0)
    {
        Inventory->AddAmmo(
            WeaponData->AmmoType,
            InitialReserveAmmo,
            FMath::Max(InitialReserveAmmo, WeaponData->MaxReserveAmmo));
    }

    if (bAutoEquip)
    {
        Inventory->EquipWeaponInSlot(StoredSlot);
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up weapon '%s' into slot %d."),
        *WeaponData->DisplayName.ToString(),
        static_cast<int32>(StoredSlot));

    return true;
}
