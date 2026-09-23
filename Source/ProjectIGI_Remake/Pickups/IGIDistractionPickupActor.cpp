#include "Pickups/IGIDistractionPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIWeaponTypes.h"

AIGIDistractionPickupActor::AIGIDistractionPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->InitSphereRadius(65.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetGenerateOverlapEvents(true);
    PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(PickupSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AIGIDistractionPickupActor::BeginPlay()
{
    Super::BeginPlay();

    PickupSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &ThisClass::HandlePickupOverlap);

    PickupSphere->UpdateOverlaps();
}

void AIGIDistractionPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    TryPickupByActor(OtherActor);
}

bool AIGIDistractionPickupActor::TryPickupByActor(AActor* OtherActor)
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

    DistractionObjectCount = FMath::Max(1, DistractionObjectCount);

    const int32 Added = Inventory->AddEquipment(
        EIGIEquipmentType::DistractionObject,
        DistractionObjectCount);

    if (Added <= 0)
    {
        return false;
    }

    DistractionObjectCount -= Added;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up %d distraction object(s). Total=%d RemainingInPickup=%d"),
        Added,
        Inventory->GetEquipmentCount(EIGIEquipmentType::DistractionObject),
        DistractionObjectCount);

    if (DistractionObjectCount <= 0)
    {
        Destroy();
    }

    return true;
}
