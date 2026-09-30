#include "AI/IGIEnemyAIController.h"

#include "Awareness/BDFRAwarenessComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Difficulty/BDFRDifficultyComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Health/IGIHealthComponent.h"
#include "Health/IGIHitReactionComponent.h"
#include "IGIPlayerCharacter.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Inventory/IGILootableInventoryComponent.h"
#include "NavigationSystem.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"
#include "Vision/IGIThermalSignatureComponent.h"

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

	if (IsValid(GetAwarenessComponent()))
	{
		GetAwarenessComponent()->OnAwarenessChanged.AddDynamic(
			this,
			&ThisClass::HandleAwarenessChanged);
	}
}

void AIGIEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!IsValid(InPawn))
	{
		return;
	}

	EnsureEnemyGameplayComponents(InPawn);
	SetTacticalState(EIGIEnemyTacticalState::Idle);
}

bool AIGIEnemyAIController::BDFR_ShouldProcessPerceivedActor_Implementation(
	AActor* SourceActor) const
{
	return TacticalState != EIGIEnemyTacticalState::Dead &&
		IsValid(SourceActor) &&
		SourceActor->IsA<AIGIPlayerCharacter>();
}

void AIGIEnemyAIController::HandleAcousticEventPerceived(
	AActor* SourceActor,
	const FName AcousticTag,
	const FVector Location,
	const float EffectiveStrength)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead ||
		!AcousticTag.ToString().StartsWith(TEXT("BDFR.Acoustic.Distraction.")))
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

	const float NormalizedStrength =
		FMath::Clamp(EffectiveStrength, 0.0f, 1.0f);

	if (TacticalState == EIGIEnemyTacticalState::Dead ||
		!IsValid(GetPawn()) ||
		NormalizedStrength < Tuning.MinimumStrength)
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
		NormalizedStrength * 0.72f +
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
	if (TacticalState == EIGIEnemyTacticalState::Dead)
	{
		return;
	}

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

	SetTacticalState(EIGIEnemyTacticalState::Investigate);

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

void AIGIEnemyAIController::HandleAwarenessChanged(
	AActor* TargetActor,
	const float Awareness,
	const EBDFRAwarenessLevel AwarenessLevel)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead)
	{
		return;
	}

	if (AwarenessLevel >= EBDFRAwarenessLevel::Alerted)
	{
		ClearDistraction();
	}

	RefreshTacticalResponse(TargetActor);
}

void AIGIEnemyAIController::HandleEnemyDeath(
	AActor* DeadActor,
	AActor* DamageCauser)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead)
	{
		return;
	}

	APawn* DeadPawn = Cast<APawn>(DeadActor);
	if (!IsValid(DeadPawn))
	{
		return;
	}

	ClearDistraction();
	ClearTacticalTimers();
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);
	SetTacticalState(EIGIEnemyTacticalState::Dead);

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		UAISense_Hearing::ReportNoiseEvent(
			World,
			DeadPawn->GetActorLocation(),
			0.90f,
			DeadPawn,
			2500.0f,
			TEXT("BDFR.Distress.AllyDown"));
	}

	ApplyDeathPresentation(DeadPawn);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("IGI enemy dead: %s DamageCauser=%s"),
		*DeadPawn->GetName(),
		*GetNameSafe(DamageCauser));
}

void AIGIEnemyAIController::HandleEnemyHitReaction(
	AActor* HitActor,
	const float Damage,
	const FVector HitLocation,
	const FVector ShotDirection,
	const FName BoneName,
	AActor* DamageCauser)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead || Damage <= 0.0f)
	{
		return;
	}

	ClearDistraction();
	StopMovement();

	if (IsValid(DamageCauser))
	{
		SetFocus(DamageCauser, EAIFocusPriority::Gameplay);
	}

	SetTacticalState(EIGIEnemyTacticalState::Combat);

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(HitReactionTimer);
		World->GetTimerManager().SetTimer(
			HitReactionTimer,
			this,
			&ThisClass::ResumeAfterHitReaction,
			FMath::Max(0.01f, HitReactionPauseSeconds),
			false);
	}

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("IGI enemy hit reaction: %s Damage=%.1f Bone=%s"),
		*GetNameSafe(HitActor),
		Damage,
		*BoneName.ToString());
}

void AIGIEnemyAIController::AdvanceSearch()
{
	if (TacticalState == EIGIEnemyTacticalState::Dead ||
		!IsValid(GetPawn()) ||
		!bEnablePrototypeTacticalMovement)
	{
		return;
	}

	const int32 TierBonus = IsValid(GetDifficultyComponent())
		? static_cast<int32>(GetDifficultyComponent()->GetDifficultyTier())
		: 1;

	const int32 EffectiveSearchPoints =
		FMath::Max(1, SearchPointCount + FMath::Clamp(TierBonus, 0, 4));

	if (SearchStepIndex >= EffectiveSearchPoints)
	{
		if (IsPersistentHuntActive())
		{
			SearchStepIndex = 0;
		}
		else
		{
			if (IsValid(GetAwarenessComponent()))
			{
				GetAwarenessComponent()->ForgetTarget();
			}

			StopMovement();
			ClearFocus(EAIFocusPriority::Gameplay);
			SetTacticalState(EIGIEnemyTacticalState::Idle);
			return;
		}
	}

	UNavigationSystemV1* NavSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	FVector Destination = SearchCenter;

	if (IsValid(NavSystem))
	{
		FNavLocation NavLocation;

		if (SearchStepIndex == 0)
		{
			if (NavSystem->ProjectPointToNavigation(
					SearchCenter,
					NavLocation,
					FVector(SearchRadius, SearchRadius, 300.0f)))
			{
				Destination = NavLocation.Location;
			}
		}
		else if (NavSystem->GetRandomReachablePointInRadius(
				SearchCenter,
				SearchRadius,
				NavLocation))
		{
			Destination = NavLocation.Location;
		}
	}

	MoveToLocation(
		Destination,
		110.0f,
		true,
		true,
		true,
		true,
		nullptr,
		true);

	++SearchStepIndex;

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().SetTimer(
			SearchStepTimer,
			this,
			&ThisClass::AdvanceSearch,
			FMath::Max(0.5f, SearchStepSeconds),
			false);
	}
}

void AIGIEnemyAIController::ResumeAfterHitReaction()
{
	if (TacticalState == EIGIEnemyTacticalState::Dead)
	{
		return;
	}

	AActor* TargetActor = IsValid(GetAwarenessComponent())
		? GetAwarenessComponent()->GetCurrentTarget()
		: nullptr;

	if (IsValid(TargetActor) && IsValid(GetAwarenessComponent()))
	{
		const FBDFRAwarenessSnapshot Snapshot =
			GetAwarenessComponent()->GetSnapshot();

		if (Snapshot.bHasLineOfSight &&
			ShouldTakeCover() &&
			TryMoveToCover(TargetActor->GetActorLocation()))
		{
			return;
		}
	}

	RefreshTacticalResponse(TargetActor);
}

void AIGIEnemyAIController::ClearDistraction()
{
	const bool bWasActive = bHasActiveDistraction;

	bHasActiveDistraction = false;
	ActiveDistractionSource = nullptr;
	ActiveDistractionLocation = FVector::ZeroVector;
	ActiveDistractionStrength = 0.0f;

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(ClearDistractionTimer);
	}

	if (bWasActive &&
		bAutoMoveToAcceptedDistraction &&
		TacticalState == EIGIEnemyTacticalState::Investigate)
	{
		StopMovement();

		const EBDFRAwarenessLevel Level = IsValid(GetAwarenessComponent())
			? GetAwarenessComponent()->GetAwarenessLevel()
			: EBDFRAwarenessLevel::Unaware;

		if (Level == EBDFRAwarenessLevel::Unaware)
		{
			SetTacticalState(EIGIEnemyTacticalState::Idle);
		}
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

void AIGIEnemyAIController::EnsureEnemyGameplayComponents(APawn* InPawn)
{
	if (!IsValid(InPawn))
	{
		return;
	}

	UIGIThermalSignatureComponent* ThermalSignature =
		InPawn->FindComponentByClass<UIGIThermalSignatureComponent>();

	if (!IsValid(ThermalSignature))
	{
		ThermalSignature =
			NewObject<UIGIThermalSignatureComponent>(
				InPawn,
				TEXT("IGIThermalSignature"));

		if (IsValid(ThermalSignature))
		{
			InPawn->AddInstanceComponent(ThermalSignature);
			ThermalSignature->RegisterComponent();
		}
	}

	UIGIHealthComponent* Health =
		InPawn->FindComponentByClass<UIGIHealthComponent>();

	if (!IsValid(Health))
	{
		Health =
			NewObject<UIGIHealthComponent>(
				InPawn,
				TEXT("IGIHealth"));

		if (IsValid(Health))
		{
			InPawn->AddInstanceComponent(Health);
			Health->RegisterComponent();
		}
	}

	if (IsValid(Health))
	{
		Health->OnDeath.RemoveDynamic(
			this,
			&ThisClass::HandleEnemyDeath);
		Health->OnDeath.AddDynamic(
			this,
			&ThisClass::HandleEnemyDeath);
	}

	UIGIHitReactionComponent* HitReaction =
		InPawn->FindComponentByClass<UIGIHitReactionComponent>();

	if (!IsValid(HitReaction))
	{
		HitReaction =
			NewObject<UIGIHitReactionComponent>(
				InPawn,
				TEXT("IGIHitReaction"));

		if (IsValid(HitReaction))
		{
			InPawn->AddInstanceComponent(HitReaction);
			HitReaction->RegisterComponent();
		}
	}

	if (IsValid(HitReaction))
	{
		HitReaction->OnHitReaction.RemoveDynamic(
			this,
			&ThisClass::HandleEnemyHitReaction);
		HitReaction->OnHitReaction.AddDynamic(
			this,
			&ThisClass::HandleEnemyHitReaction);
	}

	if (!IsValid(InPawn->FindComponentByClass<UIGIInventoryComponent>()))
	{
		UIGIInventoryComponent* Inventory =
			NewObject<UIGIInventoryComponent>(
				InPawn,
				TEXT("IGIInventory"));

		if (IsValid(Inventory))
		{
			InPawn->AddInstanceComponent(Inventory);
			Inventory->RegisterComponent();
		}
	}

	if (!IsValid(InPawn->FindComponentByClass<UIGILootableInventoryComponent>()))
	{
		UIGILootableInventoryComponent* LootableInventory =
			NewObject<UIGILootableInventoryComponent>(
				InPawn,
				TEXT("IGILootableInventory"));

		if (IsValid(LootableInventory))
		{
			InPawn->AddInstanceComponent(LootableInventory);
			LootableInventory->RegisterComponent();
		}
	}
}

void AIGIEnemyAIController::SetTacticalState(
	const EIGIEnemyTacticalState NewState)
{
	if (TacticalState == NewState)
	{
		return;
	}

	const EIGIEnemyTacticalState PreviousState = TacticalState;
	TacticalState = NewState;
	OnTacticalStateChanged.Broadcast(PreviousState, TacticalState);

	UE_LOG(
		LogTemp,
		Verbose,
		TEXT("IGI enemy tactical state: %s %d -> %d"),
		*GetName(),
		static_cast<int32>(PreviousState),
		static_cast<int32>(TacticalState));
}

void AIGIEnemyAIController::RefreshTacticalResponse(AActor* TargetActor)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead ||
		!IsValid(GetPawn()) ||
		!IsValid(GetAwarenessComponent()))
	{
		return;
	}

	const FBDFRAwarenessSnapshot Snapshot =
		GetAwarenessComponent()->GetSnapshot();

	switch (Snapshot.AwarenessLevel)
	{
		case EBDFRAwarenessLevel::ConfirmedThreat:
		case EBDFRAwarenessLevel::Alerted:
		{
			ClearDistraction();

			if (Snapshot.bHasLineOfSight && IsValid(TargetActor))
			{
				if (UWorld* World = GetWorld(); IsValid(World))
				{
					World->GetTimerManager().ClearTimer(SearchStepTimer);
				}

				SetFocus(TargetActor, EAIFocusPriority::Gameplay);

				if (TacticalState != EIGIEnemyTacticalState::TakeCover &&
					TacticalState != EIGIEnemyTacticalState::Combat &&
					ShouldTakeCover() &&
					TryMoveToCover(TargetActor->GetActorLocation()))
				{
					return;
				}

				SetTacticalState(EIGIEnemyTacticalState::Combat);

				if (bEnablePrototypeTacticalMovement)
				{
					MoveToActor(
						TargetActor,
						650.0f,
						true,
						true,
						true,
						nullptr,
						true);
				}

				return;
			}

			if (!Snapshot.LastKnownLocation.IsNearlyZero())
			{
				BeginSearch(Snapshot.LastKnownLocation);
			}
			return;
		}

		case EBDFRAwarenessLevel::Investigating:
		{
			if (!Snapshot.LastKnownLocation.IsNearlyZero())
			{
				BeginSearch(Snapshot.LastKnownLocation);
			}
			return;
		}

		case EBDFRAwarenessLevel::Suspicious:
		{
			if (!Snapshot.LastKnownLocation.IsNearlyZero() &&
				TacticalState != EIGIEnemyTacticalState::Investigate &&
				TacticalState != EIGIEnemyTacticalState::Search)
			{
				SetTacticalState(EIGIEnemyTacticalState::Investigate);

				if (bEnablePrototypeTacticalMovement)
				{
					MoveToLocation(
						Snapshot.LastKnownLocation,
						180.0f,
						true,
						true,
						true,
						true,
						nullptr,
						true);
				}
			}
			return;
		}

		case EBDFRAwarenessLevel::Unaware:
		default:
		{
			ClearFocus(EAIFocusPriority::Gameplay);

			if (!bHasActiveDistraction)
			{
				ClearTacticalTimers();
				StopMovement();
				SetTacticalState(EIGIEnemyTacticalState::Idle);
			}
			return;
		}
	}
}

void AIGIEnemyAIController::BeginSearch(
	const FVector& InSearchCenter)
{
	if (TacticalState == EIGIEnemyTacticalState::Dead ||
		InSearchCenter.IsNearlyZero())
	{
		return;
	}

	const bool bAlreadySearchingSameArea =
		TacticalState == EIGIEnemyTacticalState::Search &&
		FVector::DistSquared(SearchCenter, InSearchCenter) <=
			FMath::Square(150.0f);

	if (bAlreadySearchingSameArea)
	{
		return;
	}

	SearchCenter = InSearchCenter;
	SearchStepIndex = 0;
	SetTacticalState(EIGIEnemyTacticalState::Search);

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(SearchStepTimer);
	}

	AdvanceSearch();
}

bool AIGIEnemyAIController::TryMoveToCover(
	const FVector& ThreatLocation)
{
	FVector Candidate;

	if (!FindCoverLocation(ThreatLocation, Candidate))
	{
		return false;
	}

	CoverLocation = Candidate;
	SetTacticalState(EIGIEnemyTacticalState::TakeCover);

	if (bEnablePrototypeTacticalMovement)
	{
		MoveToLocation(
			CoverLocation,
			CoverAcceptanceRadius,
			true,
			true,
			true,
			true,
			nullptr,
			true);
	}

	return true;
}

bool AIGIEnemyAIController::FindCoverLocation(
	const FVector& ThreatLocation,
	FVector& OutCoverLocation) const
{
	APawn* ControlledPawn = GetPawn();
	UWorld* World = GetWorld();

	if (!IsValid(ControlledPawn) ||
		!IsValid(World) ||
		ThreatLocation.IsNearlyZero())
	{
		return false;
	}

	UNavigationSystemV1* NavSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);

	if (!IsValid(NavSystem))
	{
		return false;
	}

	FCollisionQueryParams QueryParams(
		SCENE_QUERY_STAT(IGICoverQuery),
		false,
		ControlledPawn);
	QueryParams.AddIgnoredActor(ControlledPawn);

	if (IsValid(GetAwarenessComponent()) &&
		IsValid(GetAwarenessComponent()->GetCurrentTarget()))
	{
		QueryParams.AddIgnoredActor(
			GetAwarenessComponent()->GetCurrentTarget());
	}

	float BestScore = TNumericLimits<float>::Max();
	bool bFoundCover = false;

	for (int32 Attempt = 0; Attempt < FMath::Max(1, CoverQueryAttempts); ++Attempt)
	{
		FNavLocation CandidateNav;

		if (!NavSystem->GetRandomReachablePointInRadius(
				ControlledPawn->GetActorLocation(),
				CoverSearchRadius,
				CandidateNav))
		{
			continue;
		}

		const FVector Candidate = CandidateNav.Location;
		const FVector TraceStart = ThreatLocation + FVector::UpVector * 70.0f;
		const FVector TraceEnd = Candidate + FVector::UpVector * 70.0f;

		FHitResult CoverHit;
		const bool bOccluded = World->LineTraceSingleByChannel(
			CoverHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			QueryParams);

		if (!bOccluded)
		{
			continue;
		}

		const float MoveDistance =
			FVector::Distance(
				ControlledPawn->GetActorLocation(),
				Candidate);

		const float ThreatDistance =
			FVector::Distance(
				ThreatLocation,
				Candidate);

		const float Score =
			MoveDistance +
			FMath::Abs(ThreatDistance - 750.0f) * 0.30f;

		if (Score < BestScore)
		{
			BestScore = Score;
			OutCoverLocation = Candidate;
			bFoundCover = true;
		}
	}

	return bFoundCover;
}

bool AIGIEnemyAIController::ShouldTakeCover() const
{
	if (TacticalState == EIGIEnemyTacticalState::Dead)
	{
		return false;
	}

	float Chance = 0.40f;

	if (IsValid(GetDifficultyComponent()))
	{
		switch (GetDifficultyComponent()->GetDifficultyTier())
		{
			case EBDFRDifficultyTier::Recruit:
				Chance = 0.22f;
				break;

			case EBDFRDifficultyTier::Private:
				Chance = 0.38f;
				break;

			case EBDFRDifficultyTier::Sergeant:
				Chance = 0.62f;
				break;

			case EBDFRDifficultyTier::Commando:
				Chance = 0.78f;
				break;

			case EBDFRDifficultyTier::SAS:
				Chance = 0.90f;
				break;
		}
	}

	if (const APawn* ControlledPawn = GetPawn(); IsValid(ControlledPawn))
	{
		if (const UIGIHealthComponent* Health =
				ControlledPawn->FindComponentByClass<UIGIHealthComponent>();
			IsValid(Health) && Health->GetHealthNormalized() < 0.55f)
		{
			Chance = FMath::Min(1.0f, Chance + 0.22f);
		}
	}

	return FMath::FRand() <= Chance;
}

void AIGIEnemyAIController::ClearTacticalTimers()
{
	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(SearchStepTimer);
		World->GetTimerManager().ClearTimer(HitReactionTimer);
	}
}

void AIGIEnemyAIController::ApplyDeathPresentation(APawn* DeadPawn)
{
	if (!IsValid(DeadPawn))
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(DeadPawn);
		IsValid(Character))
	{
		if (UCharacterMovementComponent* Movement =
				Character->GetCharacterMovement();
			IsValid(Movement))
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}

		if (UCapsuleComponent* Capsule =
				Character->GetCapsuleComponent();
			IsValid(Capsule))
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (bRagdollOnDeath)
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh();
				IsValid(Mesh))
			{
				Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				Mesh->SetAllBodiesSimulatePhysics(true);
				Mesh->SetSimulatePhysics(true);
				Mesh->WakeAllRigidBodies();
			}
		}
	}

	if (CorpseLifeSeconds > 0.0f)
	{
		DeadPawn->SetLifeSpan(CorpseLifeSeconds);
	}

	if (GetPawn() == DeadPawn)
	{
		UnPossess();
	}
}
