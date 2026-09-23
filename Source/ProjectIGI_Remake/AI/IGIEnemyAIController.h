#pragma once

#include "CoreMinimal.h"
#include "AI/IGIDistractionTypes.h"
#include "Behavior/BDFRAIController.h"
#include "Core/BDFRAITypes.h"
#include "Difficulty/BDFRDifficultyTypes.h"
#include "TimerManager.h"
#include "IGIEnemyAIController.generated.h"

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

	UPROPERTY(BlueprintAssignable, Category = "IGI|AI|Distraction")
	FIGIDistractionAcceptedSignature OnDistractionAccepted;

protected:
	virtual void BeginPlay() override;
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
};
