#include "Pickups/IGIVisionEquipmentPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/IGIInventoryComponent.h"

AIGIVisionEquipmentPickupActor::AIGIVisionEquipmentPickupActor()
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

void AIGIVisionEquipmentPickupActor::BeginPlay()
{
    Super::BeginPlay();

    PickupSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &ThisClass::HandlePickupOverlap);
    PickupSphere->UpdateOverlaps();
}

void AIGIVisionEquipmentPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    TryPickupByActor(OtherActor);
}

bool AIGIVisionEquipmentPickupActor::TryPickupByActor(AActor* OtherActor)
{
    if (!IsValid(OtherActor) || !IsSupportedVisionEquipment(VisionEquipmentType))
    {
        return false;
    }

    UIGIInventoryComponent* Inventory =
        OtherActor->FindComponentByClass<UIGIInventoryComponent>();

    if (!IsValid(Inventory))
    {
        return false;
    }

    const int32 Added = Inventory->AddEquipment(VisionEquipmentType, 1);
    if (Added != 1)
    {
        return false;
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI picked up vision equipment type %d."),
        static_cast<int32>(VisionEquipmentType));

    Destroy();
    return true;
}

bool AIGIVisionEquipmentPickupActor::IsSupportedVisionEquipment(
    const EIGIEquipmentType EquipmentType) const
{
    return EquipmentType == EIGIEquipmentType::Binoculars ||
        EquipmentType == EIGIEquipmentType::NightVisionGoggles ||
        EquipmentType == EIGIEquipmentType::ThermalViewer;
}
