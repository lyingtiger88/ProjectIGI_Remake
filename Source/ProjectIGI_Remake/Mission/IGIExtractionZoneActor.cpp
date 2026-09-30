#include "Mission/IGIExtractionZoneActor.h"

#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Mission/IGIMissionWorldSubsystem.h"

AIGIExtractionZoneActor::AIGIExtractionZoneActor()
{
    PrimaryActorTick.bCanEverTick = false;

    ExtractionRadius = CreateDefaultSubobject<USphereComponent>(TEXT("ExtractionRadius"));
    SetRootComponent(ExtractionRadius);
    ExtractionRadius->InitSphereRadius(Radius);
    ExtractionRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ExtractionRadius->SetCollisionObjectType(ECC_WorldDynamic);
    ExtractionRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
    ExtractionRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AIGIExtractionZoneActor::BeginPlay()
{
    Super::BeginPlay();

    Radius = FMath::Max(100.0f, Radius);
    ExtractionRadius->SetSphereRadius(Radius, true);

    if (UWorld* World = GetWorld(); IsValid(World))
    {
        if (UIGIMissionWorldSubsystem* Mission =
                World->GetSubsystem<UIGIMissionWorldSubsystem>();
            IsValid(Mission))
        {
            Mission->RegisterExtractionZone(
                this,
                GetActorLocation(),
                Radius);
        }
    }
}

void AIGIExtractionZoneActor::EndPlay(
    const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld(); IsValid(World))
    {
        if (UIGIMissionWorldSubsystem* Mission =
                World->GetSubsystem<UIGIMissionWorldSubsystem>();
            IsValid(Mission))
        {
            Mission->UnregisterExtractionZone(this);
        }
    }

    Super::EndPlay(EndPlayReason);
}
