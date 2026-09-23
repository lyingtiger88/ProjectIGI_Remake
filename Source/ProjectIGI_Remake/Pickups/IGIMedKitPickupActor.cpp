#include "Pickups/IGIMedKitPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIWeaponTypes.h"

AIGIMedKitPickupActor::AIGIMedKitPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->InitSphereRadius(70.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetGenerateOverlapEvents(true);
    PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(PickupSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AIGIMedKitPickupActor::BeginPlay()
{
    Super::BeginPlay();

    PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandlePickupOverlap);
    PickupSphere->UpdateOverlaps();

    TArray<AActor*> OverlappingActors;
    PickupSphere->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (TryPickupByActor(Actor) && IsActorBeingDestroyed())
        {
            return;
        }
    }
}

void AIGIMedKitPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    TryPickupByActor(OtherActor);
}

bool AIGIMedKitPickupActor::TryPickupByActor(AActor* OtherActor)
{
    if (!IsValid(OtherActor))
    {
        return false;
    }

    UIGIInventoryComponent* Inventory =
        OtherActor->FindComponentByClass<UIGIInventoryComponent>();

    if (!IsValid(Inventory))
    {
        return false;
    }

    MedKitCount = FMath::Max(1, MedKitCount);

    const int32 Added = Inventory->AddEquipment(
        EIGIEquipmentType::MedKit,
        MedKitCount);

    if (Added <= 0)
    {
        return false;
    }

    MedKitCount -= Added;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up %d Med Kit(s). Total=%d PickupRemaining=%d"),
        Added,
        Inventory->GetEquipmentCount(EIGIEquipmentType::MedKit),
        MedKitCount);

    if (MedKitCount <= 0)
    {
        Destroy();
    }

    return true;
}
