#include "Environment/IGIFlareSignalWorldSubsystem.h"

void UIGIFlareSignalWorldSubsystem::ReportFlareSignal(
    AActor* FlareActor,
    const EIGIFlarePurpose Purpose,
    const FVector Location)
{
    bHasSignal = true;
    LastPurpose = Purpose;
    LastLocation = Location;

    OnFlareSignal.Broadcast(FlareActor, Purpose, Location);
}
