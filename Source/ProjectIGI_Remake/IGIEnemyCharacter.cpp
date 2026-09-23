#include "IGIEnemyCharacter.h"

#include "IGIEnemyAIController.h"

#include "AIController.h"
#include "Acoustics/BDFRAcousticExposureComponent.h"
#include "AlsAnimationInstance.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/BDFRGameplayTags.h"
#include "Engine/SkeletalMesh.h"
#include "Health/BDFRHealthComponent.h"
#include "Health/BDFRInjuryResponseComponent.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Social/BDFRDistressComponent.h"
#include "UObject/ConstructorHelpers.h"

AIGIEnemyCharacter::AIGIEnemyCharacter()
{
	AIControllerClass = AIGIEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthComponent = CreateDefaultSubobject<UBDFRHealthComponent>(TEXT("BDFRHealth"));
	DistressComponent = CreateDefaultSubobject<UBDFRDistressComponent>(TEXT("BDFRDistress"));
	InjuryResponseComponent = CreateDefaultSubobject<UBDFRInjuryResponseComponent>(TEXT("BDFRInjuryResponse"));
	AcousticExposureComponent = CreateDefaultSubobject<UBDFRAcousticExposureComponent>(TEXT("BDFRAcousticExposure"));

	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> CharacterSettingsAsset(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (CharacterSettingsAsset.Succeeded())
	{
		Settings = CharacterSettingsAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> MovementSettingsAsset(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (MovementSettingsAsset.Succeeded())
	{
		MovementSettings = MovementSettingsAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshAsset(
		TEXT("/ALS/ALS/Character/SKM_Als.SKM_Als"));
	if (CharacterMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(CharacterMeshAsset.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnimationBlueprint(
		TEXT("/ALS/ALS/Character/AB_Als"));
	if (CharacterAnimationBlueprint.Succeeded())
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(CharacterAnimationBlueprint.Class);
	}

	GetMesh()->VisibilityBasedAnimTickOption =
		EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AIGIEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RefreshAlsAnimationInstance();
}

void AIGIEnemyCharacter::PossessedBy(AController* NewController)
{
	RefreshAlsAnimationInstance();
	Super::PossessedBy(NewController);
	RefreshAlsAnimationInstance();
}

float AIGIEnemyCharacter::TakeDamage(
	const float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (IsValid(HealthComponent))
	{
		HealthComponent->ApplyHealthDamage(
			AppliedDamage > 0.0f ? AppliedDamage : DamageAmount,
			DamageCauser);
	}

	return AppliedDamage;
}

void AIGIEnemyCharacter::BDFR_SetDesiredGait_Implementation(const FGameplayTag GaitTag)
{
	if (GaitTag == BDFRGameplayTags::Locomotion_Gait_Walking)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
	else if (GaitTag == BDFRGameplayTags::Locomotion_Gait_Sprinting)
	{
		SetDesiredGait(AlsGaitTags::Sprinting);
	}
	else
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
}

void AIGIEnemyCharacter::BDFR_SetDesiredStance_Implementation(const FGameplayTag StanceTag)
{
	SetDesiredStance(
		StanceTag == BDFRGameplayTags::Locomotion_Stance_Crouching
			? AlsStanceTags::Crouching
			: AlsStanceTags::Standing);
}

void AIGIEnemyCharacter::BDFR_SetAiming_Implementation(const bool bAiming)
{
	SetDesiredAiming(bAiming);
}

void AIGIEnemyCharacter::BDFR_SetLookTarget_Implementation(AActor* TargetActor)
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (IsValid(TargetActor))
		{
			AIController->SetFocus(TargetActor);
		}
	}
}

void AIGIEnemyCharacter::BDFR_ClearLookTarget_Implementation()
{
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

bool AIGIEnemyCharacter::BDFR_CanReceiveAssistance_Implementation(AActor* Helper) const
{
	if (!IsValid(Helper) || Helper == this || !IsValid(HealthComponent))
	{
		return false;
	}

	const FBDFRHealthSnapshot Snapshot = HealthComponent->GetHealthSnapshot();

	return Snapshot.HealthState == EBDFRHealthState::Wounded
		|| Snapshot.HealthState == EBDFRHealthState::Critical
		|| Snapshot.HealthState == EBDFRHealthState::Incapacitated
		|| Snapshot.bBleeding;
}

FVector AIGIEnemyCharacter::BDFR_GetAssistanceLocation_Implementation(AActor* Helper) const
{
	return GetActorLocation();
}

void AIGIEnemyCharacter::BDFR_BeginAssistance_Implementation(AActor* Helper)
{
	SetDesiredAiming(false);
}

void AIGIEnemyCharacter::BDFR_CompleteAssistance_Implementation(AActor* Helper)
{
	if (IsValid(HealthComponent))
	{
		HealthComponent->Stabilize(10.0f);
	}
}

void AIGIEnemyCharacter::RefreshAlsAnimationInstance()
{
	AnimationInstance = Cast<UAlsAnimationInstance>(GetMesh()->GetAnimInstance());
}
