#include "Pickups/IGIAttachmentPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIWeaponAttachmentComponent.h"
#include "Weapons/IGIWeaponAttachmentDataAsset.h"
#include "Weapons/IGIWeaponBase.h"

AIGIAttachmentPickupActor::AIGIAttachmentPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
    SetRootComponent(PickupSphere);
    PickupSphere->InitSphereRadius(75.0f);
    PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupSphere->SetCollisionObjectType(ECC_WorldDynamic);
    PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(PickupSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AIGIAttachmentPickupActor::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(PickupSphere))
    {
        PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandlePickupOverlap);
    }
}

void AIGIAttachmentPickupActor::HandlePickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (TryInstallOnActiveWeapon(OtherActor))
    {
        Destroy();
    }
}

bool AIGIAttachmentPickupActor::TryInstallOnActiveWeapon(AActor* OtherActor)
{
    AIGIPlayerCharacter* Player = Cast<AIGIPlayerCharacter>(OtherActor);
    if (!IsValid(Player) || !IsValid(AttachmentData))
    {
        return false;
    }

    UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();
    AIGIWeaponBase* ActiveWeapon = IsValid(Inventory) ? Inventory->GetActiveWeapon() : nullptr;
    UIGIWeaponAttachmentComponent* Attachments = IsValid(ActiveWeapon)
        ? ActiveWeapon->GetAttachmentComponent()
        : nullptr;

    if (!IsValid(Attachments) ||
        !Attachments->InstallAttachment(AttachmentData, bReplaceExistingAttachment))
    {
        return false;
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI installed attachment '%s' on '%s'."),
        *AttachmentData->DisplayName.ToString(),
        *ActiveWeapon->GetName());

    return true;
}
