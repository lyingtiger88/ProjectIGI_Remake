#include "AI/IGIEnemyAIController.h"

#include "Awareness/BDFRAwarenessComponent.h"
#include "Difficulty/BDFRDifficultyComponent.h"
#include "Engine/World.h"
#include "IGIPlayerCharacter.h"
#include "TimerManager.h"

AIGIEnemyAIController::AIGIEnemyAIController()
{
	RecruitDistractionTuning.MinimumStrength = 0.10f;
	RecruitDistractionTuning.ResponseThreshold = 0.18f;
	RecruitDistractionTuning.ResponseChance = 1.00f;
	RecruitDistractionTuning.MaxInvestigationDistance = 3500.0f;
	RecruitDistractionTuning.InvestigationSeconds = 10.0f;
	RecruitDistractionTuning.RepeatPenaltyPerUse = 0.07f;
	RecruitDistractionTuning.bIgnoreWhenAlerted = false;

	PrivateDistractionTuning.MinimumStrength = 0.18f;
	PrivateDistractionTuning.ResponseThreshold = 0.28f;
	PrivateDistractionTuning.ResponseChance = 0.90f;
	PrivateDistractionTuning.MaxInvestigationDistance = 3200.0f;
	PrivateDistractionTuning.InvestigationSeconds = 9.0f;
	PrivateDistractionTuning.RepeatPenaltyPerUse = 0.10f;
	PrivateDistractionTuning.bIgnoreWhenAlerted = false;

	// Tier 3+ deliberately becomes much harder to manipulate.
	SergeantDistractionTuning.MinimumStrength = 0.35f;
	SergeantDistractionTuning.ResponseThreshold = 0.52f;
	SergeantDistractionTuning.ResponseChance = 0.60f;
	SergeantDistractionTuning.MaxInvestigationDistance = 2600.0f;
	SergeantDistractionTuning.InvestigationSeconds = 7.0f;
	SergeantDistractionTuning.RepeatPenaltyPerUse = 0.16f;
	SergeantDistractionTuning.bIgnoreWhenAlerted = true;

	CommandoDistractionTuning.MinimumStrength = 0.50f;
	CommandoDistractionTuning.ResponseThreshold = 0.68f;
	CommandoDistractionTuning.ResponseChance = 0.35f;
	CommandoDistractionTuning.MaxInvestigationDistance = 2100.0f;
	CommandoDistractionTuning.InvestigationSeconds = 5.5f;
	CommandoDistractionTuning.RepeatPenaltyPerUse = 0.22f;
	CommandoDistractionTuning.bIgnoreWhenAlerted = true;

	SASDistractionTuning.MinimumStrength = 0.65f;
	SASDistractionTuning.ResponseThreshold = 0.82f;
	SASDistractionTuning.ResponseChance = 0.18f;
	SASDistractionTuning.MaxInvestigationDistance = 1600.0f;
	SASDistractionTuning.InvestigationSeconds = 4.0f;
	SASDistractionTuning.RepeatPenaltyPerUse = 0.30f;
	SASDistractionTuning.bIgnoreWhenAlerted = true;
}

void AIGIEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	OnAcousticEventPerceived.AddDynamic(
		this,
		&ThisClass::HandleAcousticEventPerceived);
}

bool AIGIEnemyAIController::BDFR_ShouldProcessPerceivedActor_Implementation(
	AActor* SourceActor) const
{
	return IsValid(SourceActor) && SourceActor->IsA<AIGIPlayerCharacter>();
}

void AIGIEnemyAIController::HandleAcousticEventPerceived(
	AActor* SourceActor,
	const FName AcousticTag,
	const FVector Location,
	const float EffectiveStrength)
{
	if (!AcousticTag.ToString().StartsWith(TEXT("BDFR.Acoustic.Distraction.")))
	{
		return;
	}

	const FIGIDistractionDifficultyTuning& Tuning = GetCurrentDistractionTuning();
	float ResponseScore = 0.0f;

	if (!ShouldAcceptDistraction(
			Location,
			EffectiveStrength,
			Tuning,
			ResponseScore))
	{
		UE_LOG(
			LogTemp,
			Verbose,
			TEXT("IGI distraction rejected: AI=%s Tier=%d Strength=%.2f Score=%.2f Repeats=%d"),
			*GetName(),
			IsValid(GetDifficultyComponent())
				? static_cast<int32>(GetDifficultyComponent()->GetDifficultyTier())
				: -1,
			EffectiveStrength,
			ResponseScore,
			RepeatedDistractionCount);
		return;
	}

	AcceptDistraction(SourceActor, Location, EffectiveStrength, Tuning);
}

bool AIGIEnemyAIController::ShouldAcceptDistraction(
	const FVector& Location,
	const float EffectiveStrength,
	const FIGIDistractionDifficultyTuning& Tuning,
	float& OutResponseScore)
{
	OutResponseScore = 0.0f;

	if (!IsValid(GetPawn()) || EffectiveStrength < Tuning.MinimumStrength)
	{
		return false;
	}

	const UBDFRAwarenessComponent* Awareness = GetAwarenessComponent();
	const EBDFRAwarenessLevel AwarenessLevel = IsValid(Awareness)
		? Awareness->GetAwarenessLevel()
		: EBDFRAwarenessLevel::Unaware;

	if (AwarenessLevel == EBDFRAwarenessLevel::ConfirmedThreat)
	{
		return false;
	}

	if (Tuning.bIgnoreWhenAlerted &&
		AwarenessLevel >= EBDFRAwarenessLevel::Alerted)
	{
		return false;
	}

	const float Distance = FVector::Distance(
		GetPawn()->GetActorLocation(),
		Location);

	if (Distance > Tuning.MaxInvestigationDistance)
	{
		return false;
	}

	UWorld* World = GetWorld();
	const float Now = IsValid(World) ? World->GetTimeSeconds() : 0.0f;

	const bool bSameArea =
		Now - LastDistractionTimeSeconds <= RepeatedDistractionMemorySeconds &&
		FVector::DistSquared(Location, LastDistractionLocation) <=
			FMath::Square(RepeatedDistractionRadius);

	if (bSameArea)
	{
		++RepeatedDistractionCount;
	}
	else
	{
		RepeatedDistractionCount = 0;
	}

	LastDistractionLocation = Location;
	LastDistractionTimeSeconds = Now;

	const float DistanceScore =
		1.0f - FMath::Clamp(
			Distance / FMath::Max(Tuning.MaxInvestigationDistance, 1.0f),
			0.0f,
			1.0f);

	const float RepeatPenalty =
		RepeatedDistractionCount * Tuning.RepeatPenaltyPerUse;

	OutResponseScore = FMath::Clamp(
		EffectiveStrength * 0.72f +
		DistanceScore * 0.28f -
		GetAwarenessPenalty(AwarenessLevel) -
		RepeatPenalty,
		0.0f,
		1.0f);

	if (OutResponseScore < Tuning.ResponseThreshold)
	{
		return false;
	}

	return FMath::FRand() <= Tuning.ResponseChance;
}

void AIGIEnemyAIController::AcceptDistraction(
	AActor* SourceActor,
	const FVector& Location,
	const float EffectiveStrength,
	const FIGIDistractionDifficultyTuning& Tuning)
{
	bHasActiveDistraction = true;
	ActiveDistractionSource = SourceActor;
	ActiveDistractionLocation = Location;
	ActiveDistractionStrength = EffectiveStrength;

	const EBDFRDifficultyTier Tier = IsValid(GetDifficultyComponent())
		? GetDifficultyComponent()->GetDifficultyTier()
		: EBDFRDifficultyTier::Private;

	OnDistractionAccepted.Broadcast(
		SourceActor,
		Location,
		EffectiveStrength,
		Tier);

	if (bAutoMoveToAcceptedDistraction)
	{
		MoveToLocation(
			Location,
			DistractionMoveAcceptanceRadius,
			true,
			true,
			true,
			true,
			nullptr,
			true);
	}

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(ClearDistractionTimer);
		World->GetTimerManager().SetTimer(
			ClearDistractionTimer,
			this,
			&ThisClass::ClearDistraction,
			FMath::Max(0.25f, Tuning.InvestigationSeconds),
			false);
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("IGI distraction accepted: AI=%s Tier=%d Strength=%.2f Location=%s"),
		*GetName(),
		static_cast<int32>(Tier),
		EffectiveStrength,
		*Location.ToCompactString());
}

void AIGIEnemyAIController::ClearDistraction()
{
	bHasActiveDistraction = false;
	ActiveDistractionSource = nullptr;
	ActiveDistractionLocation = FVector::ZeroVector;
	ActiveDistractionStrength = 0.0f;

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(ClearDistractionTimer);
	}
}

const FIGIDistractionDifficultyTuning&
AIGIEnemyAIController::GetCurrentDistractionTuning() const
{
	if (!IsValid(GetDifficultyComponent()))
	{
		return PrivateDistractionTuning;
	}

	switch (GetDifficultyComponent()->GetDifficultyTier())
	{
		case EBDFRDifficultyTier::Recruit:
			return RecruitDistractionTuning;

		case EBDFRDifficultyTier::Sergeant:
			return SergeantDistractionTuning;

		case EBDFRDifficultyTier::Commando:
			return CommandoDistractionTuning;

		case EBDFRDifficultyTier::SAS:
			return SASDistractionTuning;

		case EBDFRDifficultyTier::Private:
		default:
			return PrivateDistractionTuning;
	}
}

float AIGIEnemyAIController::GetAwarenessPenalty(
	const EBDFRAwarenessLevel AwarenessLevel)
{
	switch (AwarenessLevel)
	{
		case EBDFRAwarenessLevel::Suspicious:
			return 0.06f;

		case EBDFRAwarenessLevel::Investigating:
			return 0.16f;

		case EBDFRAwarenessLevel::Alerted:
			return 0.38f;

		case EBDFRAwarenessLevel::ConfirmedThreat:
			return 1.0f;

		case EBDFRAwarenessLevel::Unaware:
		default:
			return 0.0f;
	}
}
