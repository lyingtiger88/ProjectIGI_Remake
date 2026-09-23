#include "Pickups/IGIAttachmentPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Weapons/IGIWeaponAttachmentComponent.h"
#include "Weapons/IGIWeaponAttachmentDataAsset.h"
#include "Weapons/IGIWeaponBase.h"
#include "UObject/Package.h"

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
    UIGIWeaponAttachmentDataAsset* ResolvedAttachment = ResolveAttachmentData();

    if (!IsValid(Player) || !IsValid(ResolvedAttachment))
    {
        return false;
    }

    UIGIInventoryComponent* Inventory = Player->GetInventoryComponent();
    AIGIWeaponBase* ActiveWeapon = IsValid(Inventory) ? Inventory->GetActiveWeapon() : nullptr;
    UIGIWeaponAttachmentComponent* Attachments = IsValid(ActiveWeapon)
        ? ActiveWeapon->GetAttachmentComponent()
        : nullptr;

    if (!IsValid(Attachments) ||
        !Attachments->InstallAttachment(ResolvedAttachment, bReplaceExistingAttachment))
    {
        return false;
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI installed attachment '%s' on '%s'."),
        *ResolvedAttachment->DisplayName.ToString(),
        *ActiveWeapon->GetName());

    return true;
}

UIGIWeaponAttachmentDataAsset* AIGIAttachmentPickupActor::ResolveAttachmentData()
{
    if (IsValid(AttachmentData))
    {
        return AttachmentData;
    }

    if (IsValid(RuntimePrototypeAttachment))
    {
        return RuntimePrototypeAttachment;
    }

    if (bUsePrototypePistolSuppressor)
    {
        RuntimePrototypeAttachment = CreatePrototypePistolSuppressor();
    }

    return RuntimePrototypeAttachment;
}

UIGIWeaponAttachmentDataAsset* AIGIAttachmentPickupActor::CreatePrototypePistolSuppressor()
{
    UIGIWeaponAttachmentDataAsset* Data =
        NewObject<UIGIWeaponAttachmentDataAsset>(GetTransientPackage());

    if (!IsValid(Data))
    {
        return nullptr;
    }

    Data->AttachmentId = TEXT("Prototype.Pistol.Suppressor");
    Data->DisplayName = FText::FromString(TEXT("Pistol Suppressor"));
    Data->Mesh = PrototypeSuppressorMesh;
    Data->AttachmentSlot = EIGIAttachmentSlot::Muzzle;
    Data->AttachmentType = EIGIAttachmentType::Suppressor;
    Data->CompatibleWeaponFamilies = {EIGIWeaponFamily::Pistol};
    Data->bUniversalWhenCompatibilityEmpty = false;
    Data->RecoilMultiplier = 0.96f;
    Data->SpreadMultiplier = 0.98f;
    Data->GunshotNoiseMultiplier = 0.30f;
    Data->MovementNoiseMultiplier = 1.02f;
    Data->MuzzleFlashMultiplier = 0.20f;
    Data->WeightKg = 0.24f;
    Data->bSuppressesWeapon = true;

    return Data;
}
