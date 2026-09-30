#include "Mission/IGIObjectiveInteractableActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Mission/IGIMissionWorldSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AIGIObjectiveInteractableActor::AIGIObjectiveInteractableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    InteractionBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBounds"));
    SetRootComponent(InteractionBounds);
    InteractionBounds->SetBoxExtent(FVector(35.0f, 35.0f, 35.0f));
    InteractionBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionBounds->SetCollisionObjectType(ECC_WorldDynamic);
    InteractionBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionBounds->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    ObjectiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
    ObjectiveMesh->SetupAttachment(InteractionBounds);
    ObjectiveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ObjectiveMesh->SetRelativeScale3D(FVector(0.35f, 0.45f, 0.12f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PrototypeMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube"));

    if (PrototypeMesh.Succeeded())
    {
        ObjectiveMesh->SetStaticMesh(PrototypeMesh.Object);
    }
}

bool AIGIObjectiveInteractableActor::CanInteract_Implementation(
    AActor* Interactor)
{
    if (bCompleted || !IsValid(Interactor))
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return false;
    }

    const UIGIMissionWorldSubsystem* Mission =
        World->GetSubsystem<UIGIMissionWorldSubsystem>();

    return IsValid(Mission) &&
        Mission->GetMissionState() == EIGIMissionState::PrimaryObjective &&
        Mission->GetRequiredObjectiveId() == ObjectiveId;
}

void AIGIObjectiveInteractableActor::Interact_Implementation(
    AActor* Interactor)
{
    if (!CanInteract_Implementation(Interactor))
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    UIGIMissionWorldSubsystem* Mission =
        World->GetSubsystem<UIGIMissionWorldSubsystem>();

    if (!IsValid(Mission) ||
        !Mission->CompleteObjective(ObjectiveId, Interactor))
    {
        return;
    }

    bCompleted = true;
    InteractionBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (bHideWhenCompleted && IsValid(ObjectiveMesh))
    {
        ObjectiveMesh->SetVisibility(false, true);
    }
}

FText AIGIObjectiveInteractableActor::GetInteractionPrompt_Implementation()
{
    return InteractionPrompt;
}
