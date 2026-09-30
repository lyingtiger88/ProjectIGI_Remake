#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Mission/IGIMissionTypes.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIMissionWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FIGIMissionStateChangedSignature,
    EIGIMissionState,
    PreviousState,
    EIGIMissionState,
    NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FIGIMissionObjectiveCompletedSignature,
    FName,
    ObjectiveId,
    AActor*,
    InstigatorActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FIGIMissionCompletedSignature,
    AActor*,
    CompletionActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FIGIMissionFailedSignature,
    FName,
    FailureReason);

UCLASS(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIMissionWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    void StartMission();

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    void ConfigureMission(FName InPrimaryObjectiveId, EIGIFlarePurpose InExtractionFlarePurpose);

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    bool CompleteObjective(FName ObjectiveId, AActor* InstigatorActor);

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    void RegisterExtractionZone(AActor* ZoneActor, FVector Center, float Radius);

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    void UnregisterExtractionZone(AActor* ZoneActor);

    UFUNCTION(BlueprintCallable, Category = "IGI|Mission")
    void FailMission(FName FailureReason);

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    EIGIMissionState GetMissionState() const { return MissionState; }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    FName GetRequiredObjectiveId() const { return RequiredObjectiveId; }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    EIGIFlarePurpose GetRequiredExtractionFlarePurpose() const { return RequiredExtractionFlarePurpose; }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    bool IsPrimaryObjectiveComplete() const { return bPrimaryObjectiveComplete; }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    bool HasRegisteredExtractionZone() const { return IsValid(RegisteredExtractionZone); }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    FVector GetExtractionZoneCenter() const { return ExtractionZoneCenter; }

    UFUNCTION(BlueprintPure, Category = "IGI|Mission")
    float GetExtractionZoneRadius() const { return ExtractionZoneRadius; }

    UPROPERTY(BlueprintAssignable, Category = "IGI|Mission")
    FIGIMissionStateChangedSignature OnMissionStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Mission")
    FIGIMissionObjectiveCompletedSignature OnObjectiveCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Mission")
    FIGIMissionCompletedSignature OnMissionCompleted;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Mission")
    FIGIMissionFailedSignature OnMissionFailed;

private:
    UPROPERTY(Transient)
    EIGIMissionState MissionState = EIGIMissionState::Inactive;

    UPROPERTY(Transient)
    FName RequiredObjectiveId = TEXT("PrimaryIntel");

    UPROPERTY(Transient)
    EIGIFlarePurpose RequiredExtractionFlarePurpose = EIGIFlarePurpose::RescueExtraction;

    UPROPERTY(Transient)
    bool bPrimaryObjectiveComplete = false;

    UPROPERTY(Transient)
    TObjectPtr<AActor> RegisteredExtractionZone;

    FVector ExtractionZoneCenter = FVector::ZeroVector;
    float ExtractionZoneRadius = 700.0f;

    UFUNCTION()
    void HandleFlareSignal(
        AActor* FlareActor,
        EIGIFlarePurpose Purpose,
        FVector Location);

    void SetMissionState(EIGIMissionState NewState);
    void ShowMissionMessage(const FString& Message) const;
};
