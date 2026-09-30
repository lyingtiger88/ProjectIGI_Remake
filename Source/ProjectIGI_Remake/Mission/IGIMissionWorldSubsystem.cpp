#include "Mission/IGIMissionWorldSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Environment/IGIFlareSignalWorldSubsystem.h"
#include "IGIPlayerCharacter.h"

void UIGIMissionWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    if (UIGIFlareSignalWorldSubsystem* FlareSignals =
            InWorld.GetSubsystem<UIGIFlareSignalWorldSubsystem>();
        IsValid(FlareSignals))
    {
        FlareSignals->OnFlareSignal.AddDynamic(
            this,
            &ThisClass::HandleFlareSignal);
    }

}

void UIGIMissionWorldSubsystem::Deinitialize()
{
    if (UWorld* World = GetWorld(); IsValid(World))
    {
        if (UIGIFlareSignalWorldSubsystem* FlareSignals =
                World->GetSubsystem<UIGIFlareSignalWorldSubsystem>();
            IsValid(FlareSignals))
        {
            FlareSignals->OnFlareSignal.RemoveDynamic(
                this,
                &ThisClass::HandleFlareSignal);
        }
    }

    Super::Deinitialize();
}

void UIGIMissionWorldSubsystem::StartMission()
{
    bPrimaryObjectiveComplete = false;
    SetMissionState(EIGIMissionState::PrimaryObjective);

    ShowMissionMessage(
        FString::Printf(
            TEXT("PRIMARY OBJECTIVE: Interact with '%s'."),
            *RequiredObjectiveId.ToString()));
}

void UIGIMissionWorldSubsystem::ConfigureMission(
    const FName InPrimaryObjectiveId,
    const EIGIFlarePurpose InExtractionFlarePurpose)
{
    if (!InPrimaryObjectiveId.IsNone())
    {
        RequiredObjectiveId = InPrimaryObjectiveId;
    }

    RequiredExtractionFlarePurpose = InExtractionFlarePurpose;
}

bool UIGIMissionWorldSubsystem::CompleteObjective(
    const FName ObjectiveId,
    AActor* InstigatorActor)
{
    if (MissionState != EIGIMissionState::PrimaryObjective ||
        ObjectiveId != RequiredObjectiveId)
    {
        return false;
    }

    bPrimaryObjectiveComplete = true;
    OnObjectiveCompleted.Broadcast(ObjectiveId, InstigatorActor);

    SetMissionState(EIGIMissionState::Extraction);
    ShowMissionMessage(
        TEXT("OBJECTIVE COMPLETE: Reach extraction and fire a Rescue/Extraction flare."));

    return true;
}

void UIGIMissionWorldSubsystem::RegisterExtractionZone(
    AActor* ZoneActor,
    const FVector Center,
    const float Radius)
{
    if (!IsValid(ZoneActor))
    {
        return;
    }

    RegisteredExtractionZone = ZoneActor;
    ExtractionZoneCenter = Center;
    ExtractionZoneRadius = FMath::Max(100.0f, Radius);

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI mission extraction zone registered: %s Center=%s Radius=%.0f"),
        *ZoneActor->GetName(),
        *ExtractionZoneCenter.ToCompactString(),
        ExtractionZoneRadius);
}

void UIGIMissionWorldSubsystem::UnregisterExtractionZone(AActor* ZoneActor)
{
    if (RegisteredExtractionZone != ZoneActor)
    {
        return;
    }

    RegisteredExtractionZone = nullptr;
    ExtractionZoneCenter = FVector::ZeroVector;
}

void UIGIMissionWorldSubsystem::FailMission(const FName FailureReason)
{
    if (MissionState == EIGIMissionState::Inactive ||
        MissionState == EIGIMissionState::Completed ||
        MissionState == EIGIMissionState::Failed)
    {
        return;
    }

    SetMissionState(EIGIMissionState::Failed);
    OnMissionFailed.Broadcast(FailureReason);

    ShowMissionMessage(
        FString::Printf(
            TEXT("MISSION FAILED: %s"),
            *FailureReason.ToString()));
}

void UIGIMissionWorldSubsystem::HandleFlareSignal(
    AActor* FlareActor,
    const EIGIFlarePurpose Purpose,
    const FVector Location)
{
    if (MissionState != EIGIMissionState::Extraction ||
        Purpose != RequiredExtractionFlarePurpose ||
        !IsValid(RegisteredExtractionZone))
    {
        return;
    }

    AActor* CompletionActor = nullptr;

    if (IsValid(FlareActor))
    {
        CompletionActor = FlareActor->GetInstigator();

        if (!IsValid(CompletionActor))
        {
            CompletionActor = FlareActor->GetOwner();
        }
    }

    if (!IsValid(CompletionActor) ||
        !CompletionActor->IsA<AIGIPlayerCharacter>())
    {
        return;
    }

    if (FVector::DistSquared(Location, ExtractionZoneCenter) >
        FMath::Square(ExtractionZoneRadius))
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("IGI extraction flare ignored: outside extraction zone. Location=%s"),
            *Location.ToCompactString());
        return;
    }

    SetMissionState(EIGIMissionState::Completed);
    OnMissionCompleted.Broadcast(CompletionActor);
    ShowMissionMessage(TEXT("MISSION COMPLETE: Extraction signal confirmed."));
}

void UIGIMissionWorldSubsystem::SetMissionState(
    const EIGIMissionState NewState)
{
    if (MissionState == NewState)
    {
        return;
    }

    const EIGIMissionState PreviousState = MissionState;
    MissionState = NewState;

    OnMissionStateChanged.Broadcast(PreviousState, MissionState);

    UE_LOG(
        LogTemp,
        Log,
        TEXT("IGI mission state: %d -> %d"),
        static_cast<int32>(PreviousState),
        static_cast<int32>(MissionState));
}

void UIGIMissionWorldSubsystem::ShowMissionMessage(
    const FString& Message) const
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

    if (GEngine != nullptr)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            6.0f,
            FColor::Cyan,
            Message);
    }
}
