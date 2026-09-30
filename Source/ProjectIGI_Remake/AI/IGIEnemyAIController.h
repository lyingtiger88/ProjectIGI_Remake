#pragma once

#include "CoreMinimal.h"
#include "AI/IGIDistractionTypes.h"
#include "AI/IGIEnemyTacticalTypes.h"
#include "Behavior/BDFRAIController.h"
#include "Core/BDFRAITypes.h"
#include "Difficulty/BDFRDifficultyTypes.h"
#include "TimerManager.h"
#include "IGIEnemyAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FIGIEnemyTacticalStateChangedSignature,
	EIGIEnemyTacticalState,
	PreviousState,
	EIGIEnemyTacticalState,
	NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FIGIDistractionAcceptedSignature,
	AActor*,
	SourceObject,
	FVector,
	InvestigationLocation,
	float,
	EffectiveStrength,
	EBDFRDifficultyTier,
	DifficultyTier);

UCLASS(Blueprintable)
class PROJECTIGI_REMAKE_API AIGIEnemyAIController : public ABDFRAIController
{
	GENERATED_BODY()

public:
	AIGIEnemyAIController();

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Distraction")
	bool HasActiveDistraction() const { return bHasActiveDistraction; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Distraction")
	FVector GetDistractionLocation() const { return ActiveDistractionLocation; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Distraction")
	float GetDistractionStrength() const { return ActiveDistractionStrength; }

	UFUNCTION(BlueprintCallable, Category = "IGI|AI|Distraction")
	void ClearDistraction();

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Tactical")
	EIGIEnemyTacticalState GetTacticalState() const { return TacticalState; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Tactical")
	FVector GetSearchCenter() const { return SearchCenter; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Tactical")
	FVector GetCoverLocation() const { return CoverLocation; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Tactical")
	bool IsDead() const { return TacticalState == EIGIEnemyTacticalState::Dead; }

	UPROPERTY(BlueprintAssignable, Category = "IGI|AI|Distraction")
	FIGIDistractionAcceptedSignature OnDistractionAccepted;

	UPROPERTY(BlueprintAssignable, Category = "IGI|AI|Tactical")
	FIGIEnemyTacticalStateChangedSignature OnTacticalStateChanged;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual bool BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction")
	bool bAutoMoveToAcceptedDistraction = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction", meta = (ClampMin = "10.0", ForceUnits = "cm"))
	float DistractionMoveAcceptanceRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction", meta = (ClampMin = "100.0", ForceUnits = "cm"))
	float RepeatedDistractionRadius = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction", meta = (ClampMin = "1.0", ForceUnits = "s"))
	float RepeatedDistractionMemorySeconds = 22.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction|Difficulty")
	FIGIDistractionDifficultyTuning RecruitDistractionTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction|Difficulty")
	FIGIDistractionDifficultyTuning PrivateDistractionTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction|Difficulty")
	FIGIDistractionDifficultyTuning SergeantDistractionTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction|Difficulty")
	FIGIDistractionDifficultyTuning CommandoDistractionTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Distraction|Difficulty")
	FIGIDistractionDifficultyTuning SASDistractionTuning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical")
	bool bEnablePrototypeTacticalMovement = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Search", meta = (ClampMin = "100.0", ForceUnits = "cm"))
	float SearchRadius = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Search", meta = (ClampMin = "1", ClampMax = "12"))
	int32 SearchPointCount = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Search", meta = (ClampMin = "0.5", ForceUnits = "s"))
	float SearchStepSeconds = 2.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Cover", meta = (ClampMin = "200.0", ForceUnits = "cm"))
	float CoverSearchRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Cover", meta = (ClampMin = "10.0", ForceUnits = "cm"))
	float CoverAcceptanceRadius = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Tactical|Cover", meta = (ClampMin = "1", ClampMax = "24"))
	int32 CoverQueryAttempts = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Damage", meta = (ClampMin = "0.0", ForceUnits = "s"))
	float HitReactionPauseSeconds = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Death")
	bool bRagdollOnDeath = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|AI|Death", meta = (ClampMin = "0.0", ForceUnits = "s"))
	float CorpseLifeSeconds = 0.0f;

private:
	UPROPERTY(Transient)
	bool bHasActiveDistraction = false;

	UPROPERTY(Transient)
	FVector ActiveDistractionLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	float ActiveDistractionStrength = 0.0f;

	UPROPERTY(Transient)
	TObjectPtr<AActor> ActiveDistractionSource;

	FVector LastDistractionLocation = FVector::ZeroVector;
	float LastDistractionTimeSeconds = -1000.0f;
	int32 RepeatedDistractionCount = 0;

	FTimerHandle ClearDistractionTimer;
	FTimerHandle SearchStepTimer;
	FTimerHandle HitReactionTimer;

	UPROPERTY(Transient)
	EIGIEnemyTacticalState TacticalState = EIGIEnemyTacticalState::Idle;

	UPROPERTY(Transient)
	FVector SearchCenter = FVector::ZeroVector;

	UPROPERTY(Transient)
	FVector CoverLocation = FVector::ZeroVector;

	int32 SearchStepIndex = 0;

	UFUNCTION()
	void HandleAcousticEventPerceived(
		AActor* SourceActor,
		FName AcousticTag,
		FVector Location,
		float EffectiveStrength);

	UFUNCTION()
	void HandleAwarenessChanged(
		AActor* TargetActor,
		float Awareness,
		EBDFRAwarenessLevel AwarenessLevel);

	UFUNCTION()
	void HandleEnemyDeath(AActor* DeadActor, AActor* DamageCauser);

	UFUNCTION()
	void HandleEnemyHitReaction(
		AActor* HitActor,
		float Damage,
		FVector HitLocation,
		FVector ShotDirection,
		FName BoneName,
		AActor* DamageCauser);

	UFUNCTION()
	void AdvanceSearch();

	UFUNCTION()
	void ResumeAfterHitReaction();

	bool ShouldAcceptDistraction(
		const FVector& Location,
		float EffectiveStrength,
		const FIGIDistractionDifficultyTuning& Tuning,
		float& OutResponseScore);

	void AcceptDistraction(
		AActor* SourceActor,
		const FVector& Location,
		float EffectiveStrength,
		const FIGIDistractionDifficultyTuning& Tuning);

	const FIGIDistractionDifficultyTuning& GetCurrentDistractionTuning() const;
	static float GetAwarenessPenalty(EBDFRAwarenessLevel AwarenessLevel);

	void EnsureEnemyGameplayComponents(APawn* InPawn);
	void SetTacticalState(EIGIEnemyTacticalState NewState);
	void RefreshTacticalResponse(AActor* TargetActor);
	void BeginSearch(const FVector& InSearchCenter);
	bool TryMoveToCover(const FVector& ThreatLocation);
	bool FindCoverLocation(const FVector& ThreatLocation, FVector& OutCoverLocation) const;
	bool ShouldTakeCover() const;
	void ClearTacticalTimers();
	void ApplyDeathPresentation(APawn* DeadPawn);
};
