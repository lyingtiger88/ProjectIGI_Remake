#include "Mission/IGIVerticalSliceMissionDirectorActor.h"

#include "Engine/World.h"
#include "Mission/IGIMissionWorldSubsystem.h"

AIGIVerticalSliceMissionDirectorActor::AIGIVerticalSliceMissionDirectorActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AIGIVerticalSliceMissionDirectorActor::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    UIGIMissionWorldSubsystem* Mission =
        World->GetSubsystem<UIGIMissionWorldSubsystem>();

    if (!IsValid(Mission))
    {
        return;
    }

    Mission->ConfigureMission(
        PrimaryObjectiveId,
        ExtractionFlarePurpose);

    if (bAutoStartMission)
    {
        Mission->StartMission();
    }
}
