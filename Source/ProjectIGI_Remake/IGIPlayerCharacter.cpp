#include "IGIPlayerCharacter.h"

#include "AlsAnimationInstance.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Combat/IGICombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Inventory/IGIInventoryComponent.h"
#include "InputCoreTypes.h"
#include "Math/RotationMatrix.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "Stealth/IGIAcousticSignatureComponent.h"
#include "Tracking/BDFRTrackEmitterComponent.h"
#include "Tracking/IGITrackingSurfaceComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/IGIFirearmBase.h"

AIGIPlayerCharacter::AIGIPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	TrackEmitterComponent = CreateDefaultSubobject<UBDFRTrackEmitterComponent>(TEXT("BDFRTrackEmitter"));
	TrackingSurfaceComponent = CreateDefaultSubobject<UIGITrackingSurfaceComponent>(TEXT("IGITrackingSurface"));
	CombatComponent = CreateDefaultSubobject<UIGICombatComponent>(TEXT("IGICombat"));
	InventoryComponent = CreateDefaultSubobject<UIGIInventoryComponent>(TEXT("IGIInventory"));
	AcousticSignatureComponent = CreateDefaultSubobject<UIGIAcousticSignatureComponent>(TEXT("IGIAcousticSignature"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = HipCameraArmLength;
	CameraBoom->SocketOffset = StandingCameraOffset;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 14.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->FieldOfView = HipFieldOfView;

	static ConstructorHelpers::FObjectFinder<UAlsCharacterSettings> CharacterSettingsAsset(
		TEXT("/ALS/ALS/Data/Character/CS_Als_Default.CS_Als_Default"));
	if (CharacterSettingsAsset.Succeeded()) { Settings = CharacterSettingsAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UAlsMovementSettings> MovementSettingsAsset(
		TEXT("/ALS/ALS/Data/Character/Movement/MS_Als_Normal.MS_Als_Normal"));
	if (MovementSettingsAsset.Succeeded()) { MovementSettings = MovementSettingsAsset.Object; }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshAsset(
		TEXT("/ALS/ALS/Character/SKM_Als.SKM_Als"));
	if (CharacterMeshAsset.Succeeded()) { GetMesh()->SetSkeletalMeshAsset(CharacterMeshAsset.Object); }

	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnimationBlueprint(TEXT("/ALS/ALS/Character/AB_Als"));
	if (CharacterAnimationBlueprint.Succeeded())
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(CharacterAnimationBlueprint.Class);
	}

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingAsset(
		TEXT("/ALS/ALS/Data/Input/IMC_Als_Default.IMC_Als_Default"));
	if (InputMappingAsset.Succeeded()) { InputMappingContext = InputMappingAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> LookMouseInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_LookMouse.IA_Als_LookMouse"));
	if (LookMouseInputAsset.Succeeded()) { LookMouseAction = LookMouseInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> LookInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Look.IA_Als_Look"));
	if (LookInputAsset.Succeeded()) { LookAction = LookInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Move.IA_Als_Move"));
	if (MoveInputAsset.Succeeded()) { MoveAction = MoveInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> SprintInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Sprint.IA_Als_Sprint"));
	if (SprintInputAsset.Succeeded()) { SprintAction = SprintInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> WalkInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Walk.IA_Als_Walk"));
	if (WalkInputAsset.Succeeded()) { WalkAction = WalkInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Crouch.IA_Als_Crouch"));
	if (CrouchInputAsset.Succeeded()) { CrouchAction = CrouchInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Jump.IA_Als_Jump"));
	if (JumpInputAsset.Succeeded()) { JumpAction = JumpInputAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> AimInputAsset(
		TEXT("/ALS/ALS/Data/Input/IA_Als_Aim.IA_Als_Aim"));
	if (AimInputAsset.Succeeded()) { AimAction = AimInputAsset.Object; }
}

void AIGIPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	RefreshAlsAnimationInstance();
	RefreshInputMappingContext();

	if (IsValid(GetCapsuleComponent()))
	{
		StandingCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement(); IsValid(Movement))
	{
		CrouchedCapsuleHalfHeight = Movement->GetCrouchedHalfHeight();
	}

	PlayerStance = GetStance() == AlsStanceTags::Crouching
		? EIGIPlayerStance::Crouching
		: EIGIPlayerStance::Standing;
}

void AIGIPlayerCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RefreshCameraPresentation(DeltaSeconds);
}

void AIGIPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RefreshAlsAnimationInstance();
}

void AIGIPlayerCharacter::PossessedBy(AController* NewController)
{
	RefreshAlsAnimationInstance();
	Super::PossessedBy(NewController);
	RefreshAlsAnimationInstance();
}

void AIGIPlayerCharacter::NotifyControllerChanged()
{
	if (auto* PreviousPlayer = Cast<APlayerController>(PreviousController); IsValid(PreviousPlayer))
	{
		RemoveInputMappingContext(PreviousPlayer);
	}

	Super::NotifyControllerChanged();
	RefreshInputMappingContext();
}

void AIGIPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	RefreshInputMappingContext();
}

void AIGIPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInput))
	{
		UE_LOG(LogTemp, Error, TEXT("IGI player did not receive an EnhancedInputComponent."));
		return;
	}

	if (IsValid(LookMouseAction))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnLookMouse);
	}
	if (IsValid(LookAction))
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnLook);
	}
	if (IsValid(MoveAction))
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnMove);
	}
	if (IsValid(SprintAction))
	{
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnSprint);
	}
	if (IsValid(WalkAction))
	{
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
	}
	if (IsValid(CrouchAction))
	{
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::Input_OnStancePressed);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::Input_OnStanceReleased);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnStanceReleased);
	}
	if (IsValid(JumpAction))
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnJump);
	}
	if (IsValid(AimAction))
	{
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnAim);
	}

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ThisClass::Input_OnFire);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &ThisClass::Input_OnReload);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &ThisClass::Input_OnEquipWeapon01);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &ThisClass::Input_OnEquipWeapon02);
	PlayerInputComponent->BindKey(EKeys::Three, IE_Pressed, this, &ThisClass::Input_OnEquipWeapon03);
	PlayerInputComponent->BindKey(EKeys::Four, IE_Pressed, this, &ThisClass::Input_OnEquipWeapon04);
	PlayerInputComponent->BindKey(EKeys::Five, IE_Pressed, this, &ThisClass::Input_OnEquipKnife);
	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &ThisClass::Input_OnSwitchShoulder);
}

void AIGIPlayerCharacter::Landed(const FHitResult& Hit)
{
	const float VerticalSpeed = FMath::Abs(GetVelocity().Z);
	Super::Landed(Hit);

	if (IsValid(AcousticSignatureComponent))
	{
		const float LandingIntensity = FMath::GetMappedRangeValueClamped(
			FVector2D(150.0f, 900.0f),
			FVector2D(0.55f, 1.75f),
			VerticalSpeed);

		AcousticSignatureComponent->ReportLanding(LandingIntensity);
	}
}

float AIGIPlayerCharacter::GetProneMovementDirectionAngle() const
{
	if (!IsProne())
	{
		return 0.0f;
	}

	const FVector LocalVelocity = GetActorTransform().InverseTransformVectorNoScale(GetVelocity());
	return FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
}

float AIGIPlayerCharacter::GetProneNormalizedSpeed() const
{
	if (!IsProne())
	{
		return 0.0f;
	}

	const FVector Velocity = GetVelocity();
	const float PlanarSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	const float ReferenceSpeed = 375.0f * FMath::Max(ProneMovementInputScale, 0.05f);
	return FMath::Clamp(PlanarSpeed / ReferenceSpeed, 0.0f, 1.0f);
}

bool AIGIPlayerCharacter::SetPlayerStance(const EIGIPlayerStance NewStance)
{
	if (PlayerStance == NewStance)
	{
		return true;
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (!IsValid(Movement) || !IsValid(Capsule) || !Movement->IsMovingOnGround())
	{
		return false;
	}

	const EIGIPlayerStance PreviousStance = PlayerStance;

	switch (NewStance)
	{
		case EIGIPlayerStance::Standing:
		{
			if (!CanExpandCapsuleTo(StandingCapsuleHalfHeight))
			{
				return false;
			}

			Movement->SetCrouchedHalfHeight(CrouchedCapsuleHalfHeight);
			SetDesiredStance(AlsStanceTags::Standing);
			UnCrouch(false);
			break;
		}

		case EIGIPlayerStance::Crouching:
		{
			Movement->SetCrouchedHalfHeight(CrouchedCapsuleHalfHeight);

			if (PreviousStance == EIGIPlayerStance::Prone &&
				!ResizeCapsuleKeepingFeet(CrouchedCapsuleHalfHeight))
			{
				Movement->SetCrouchedHalfHeight(ProneCapsuleHalfHeight);
				return false;
			}

			SetDesiredStance(AlsStanceTags::Crouching);
			Crouch(false);
			break;
		}

		case EIGIPlayerStance::Prone:
		{
			if (GetCharacterMovement()->IsFalling())
			{
				return false;
			}

			Movement->SetCrouchedHalfHeight(ProneCapsuleHalfHeight);
			SetDesiredStance(AlsStanceTags::Crouching);
			Crouch(false);

			if (!ResizeCapsuleKeepingFeet(ProneCapsuleHalfHeight))
			{
				Movement->SetCrouchedHalfHeight(CrouchedCapsuleHalfHeight);
				return false;
			}

			SetDesiredGait(AlsGaitTags::Walking);
			break;
		}
	}

	PlayerStance = NewStance;
	OnPlayerStanceChanged.Broadcast(PreviousStance, PlayerStance);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("IGI stance changed: %d -> %d"),
		static_cast<int32>(PreviousStance),
		static_cast<int32>(PlayerStance));

	return true;
}

void AIGIPlayerCharacter::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const FVector2D Value = ActionValue.Get<FVector2D>();
	AddControllerYawInput(Value.X);
	AddControllerPitchInput(Value.Y);
}

void AIGIPlayerCharacter::Input_OnLook(const FInputActionValue& ActionValue)
{
	const FVector2D Value = ActionValue.Get<FVector2D>();
	const float DeltaSeconds = GetWorld() != nullptr ? GetWorld()->GetDeltaSeconds() : 0.0f;
	AddControllerYawInput(Value.X * 240.0f * DeltaSeconds);
	AddControllerPitchInput(Value.Y * 90.0f * DeltaSeconds);
}

void AIGIPlayerCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
	if (!IsValid(GetController()))
	{
		return;
	}

	FVector2D Value = ActionValue.Get<FVector2D>();

	if (IsProne())
	{
		Value *= ProneMovementInputScale;
	}

	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0.0, ControlRotation.Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void AIGIPlayerCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
	const bool bSprint = ActionValue.Get<bool>();

	if (bSprint && PlayerStance != EIGIPlayerStance::Standing)
	{
		if (!SetPlayerStance(EIGIPlayerStance::Standing))
		{
			return;
		}
	}

	SetDesiredGait(bSprint ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void AIGIPlayerCharacter::Input_OnWalk()
{
	if (IsProne())
	{
		SetDesiredGait(AlsGaitTags::Walking);
		return;
	}

	SetDesiredGait(GetDesiredGait() == AlsGaitTags::Walking ? AlsGaitTags::Running : AlsGaitTags::Walking);
}

void AIGIPlayerCharacter::Input_OnStancePressed()
{
	bStanceHoldTriggered = false;

	if (PlayerStance == EIGIPlayerStance::Prone)
	{
		bStanceHoldTriggered = true;
		SetPlayerStance(EIGIPlayerStance::Crouching);
		return;
	}

	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().SetTimer(
			StanceHoldTimer,
			this,
			&ThisClass::TriggerProneFromStanceHold,
			HoldCrouchToProneSeconds,
			false);
	}
}

void AIGIPlayerCharacter::Input_OnStanceReleased()
{
	if (UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(StanceHoldTimer);
	}

	if (bStanceHoldTriggered)
	{
		return;
	}

	if (PlayerStance == EIGIPlayerStance::Standing)
	{
		SetPlayerStance(EIGIPlayerStance::Crouching);
	}
	else if (PlayerStance == EIGIPlayerStance::Crouching)
	{
		SetPlayerStance(EIGIPlayerStance::Standing);
	}
}

void AIGIPlayerCharacter::TriggerProneFromStanceHold()
{
	bStanceHoldTriggered = SetPlayerStance(EIGIPlayerStance::Prone);
}

void AIGIPlayerCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (PlayerStance == EIGIPlayerStance::Prone)
		{
			SetPlayerStance(EIGIPlayerStance::Crouching);
			return;
		}

		if (PlayerStance == EIGIPlayerStance::Crouching)
		{
			SetPlayerStance(EIGIPlayerStance::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

void AIGIPlayerCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
	bAimInputHeld = ActionValue.Get<bool>();
	SetDesiredAiming(bAimInputHeld);

	if (IsValid(CombatComponent))
	{
		if (bAimInputHeld)
		{
			CombatComponent->StartAim();
		}
		else
		{
			CombatComponent->StopAim();
		}
	}
}

void AIGIPlayerCharacter::Input_OnFire()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	AIGIFirearmBase* Firearm = Cast<AIGIFirearmBase>(InventoryComponent->GetActiveWeapon());
	if (!IsValid(Firearm))
	{
		return;
	}

	if (IsValid(CombatComponent))
	{
		CombatComponent->SetCombatState(EIGICombatState::Firing);
	}

	const bool bFired = Firearm->FireHitscan(GetController());

	if (IsValid(CombatComponent))
	{
		CombatComponent->SetCombatState(
			CombatComponent->IsAiming()
				? EIGICombatState::Aiming
				: EIGICombatState::Armed);
	}

	if (bFired)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("IGI fired %s. Magazine: %d/%d"),
			*Firearm->GetName(),
			Firearm->GetCurrentMagazineAmmo(),
			Firearm->GetMagazineCapacity());
	}
}

void AIGIPlayerCharacter::Input_OnReload()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	AIGIFirearmBase* Firearm = Cast<AIGIFirearmBase>(InventoryComponent->GetActiveWeapon());
	if (!IsValid(Firearm))
	{
		return;
	}

	if (IsValid(CombatComponent) && !CombatComponent->BeginReload())
	{
		return;
	}

	const int32 ReloadedRounds = Firearm->ReloadFromInventory(InventoryComponent);

	if (IsValid(CombatComponent))
	{
		CombatComponent->EndReload();
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("IGI reload %s: +%d rounds, magazine %d/%d."),
		*Firearm->GetName(),
		ReloadedRounds,
		Firearm->GetCurrentMagazineAmmo(),
		Firearm->GetMagazineCapacity());
}

void AIGIPlayerCharacter::Input_OnEquipWeapon01()
{
	EquipInventorySlot(EIGICarrySlot::Weapon01);
}

void AIGIPlayerCharacter::Input_OnEquipWeapon02()
{
	EquipInventorySlot(EIGICarrySlot::Weapon02);
}

void AIGIPlayerCharacter::Input_OnEquipWeapon03()
{
	EquipInventorySlot(EIGICarrySlot::Weapon03);
}

void AIGIPlayerCharacter::Input_OnEquipWeapon04()
{
	EquipInventorySlot(EIGICarrySlot::Weapon04);
}

void AIGIPlayerCharacter::Input_OnEquipKnife()
{
	EquipInventorySlot(EIGICarrySlot::Knife);
}

void AIGIPlayerCharacter::Input_OnSwitchShoulder()
{
	bRightShoulderCamera = !bRightShoulderCamera;
}

void AIGIPlayerCharacter::EquipInventorySlot(const EIGICarrySlot Slot)
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->EquipWeaponInSlot(Slot);
	}
}

bool AIGIPlayerCharacter::CanExpandCapsuleTo(const float TargetHalfHeight) const
{
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	UWorld* World = GetWorld();

	if (!IsValid(Capsule) || !IsValid(World))
	{
		return false;
	}

	const float CurrentHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();

	if (TargetHalfHeight <= CurrentHalfHeight + KINDA_SMALL_NUMBER)
	{
		return true;
	}

	const float Radius = Capsule->GetUnscaledCapsuleRadius();
	const FVector TargetLocation =
		GetActorLocation() + FVector::UpVector * (TargetHalfHeight - CurrentHalfHeight);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(IGIStanceClearance), false, this);
	QueryParams.AddIgnoredActor(this);

	return !World->OverlapBlockingTestByChannel(
		TargetLocation,
		FQuat::Identity,
		Capsule->GetCollisionObjectType(),
		FCollisionShape::MakeCapsule(Radius, TargetHalfHeight),
		QueryParams);
}

bool AIGIPlayerCharacter::ResizeCapsuleKeepingFeet(const float TargetHalfHeight)
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (!IsValid(Capsule))
	{
		return false;
	}

	const float CurrentHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();

	if (FMath::IsNearlyEqual(CurrentHalfHeight, TargetHalfHeight, 0.1f))
	{
		return true;
	}

	if (TargetHalfHeight > CurrentHalfHeight && !CanExpandCapsuleTo(TargetHalfHeight))
	{
		return false;
	}

	const float HeightDelta = TargetHalfHeight - CurrentHalfHeight;

	if (HeightDelta > 0.0f)
	{
		SetActorLocation(GetActorLocation() + FVector::UpVector * HeightDelta, false);
		Capsule->SetCapsuleHalfHeight(TargetHalfHeight, true);
	}
	else
	{
		Capsule->SetCapsuleHalfHeight(TargetHalfHeight, true);
		SetActorLocation(GetActorLocation() + FVector::UpVector * HeightDelta, false);
	}

	return true;
}

void AIGIPlayerCharacter::RefreshCameraPresentation(const float DeltaSeconds)
{
	if (!IsValid(CameraBoom) || !IsValid(FollowCamera))
	{
		return;
	}

	FVector TargetOffset = GetTargetCameraOffset();
	TargetOffset.Y = FMath::Abs(TargetOffset.Y) * (bRightShoulderCamera ? 1.0f : -1.0f);

	CameraBoom->SocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		TargetOffset,
		DeltaSeconds,
		CameraTransitionSpeed);

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		GetTargetCameraArmLength(),
		DeltaSeconds,
		CameraTransitionSpeed);

	FollowCamera->FieldOfView = FMath::FInterpTo(
		FollowCamera->FieldOfView,
		GetTargetCameraFieldOfView(),
		DeltaSeconds,
		CameraTransitionSpeed);
}

FVector AIGIPlayerCharacter::GetTargetCameraOffset() const
{
	if (bAimInputHeld)
	{
		switch (PlayerStance)
		{
			case EIGIPlayerStance::Crouching: return CrouchingAimCameraOffset;
			case EIGIPlayerStance::Prone: return ProneAimCameraOffset;
			case EIGIPlayerStance::Standing:
			default: return StandingAimCameraOffset;
		}
	}

	switch (PlayerStance)
	{
		case EIGIPlayerStance::Crouching: return CrouchingCameraOffset;
		case EIGIPlayerStance::Prone: return ProneCameraOffset;
		case EIGIPlayerStance::Standing:
		default: return StandingCameraOffset;
	}
}

float AIGIPlayerCharacter::GetTargetCameraArmLength() const
{
	if (bAimInputHeld)
	{
		return IsProne() ? ProneAimCameraArmLength : AimCameraArmLength;
	}

	return IsProne() ? ProneHipCameraArmLength : HipCameraArmLength;
}

float AIGIPlayerCharacter::GetTargetCameraFieldOfView() const
{
	if (!bAimInputHeld)
	{
		return HipFieldOfView;
	}

	return IsProne() ? ProneAimFieldOfView : AimFieldOfView;
}

void AIGIPlayerCharacter::RefreshAlsAnimationInstance()
{
	AnimationInstance = Cast<UAlsAnimationInstance>(GetMesh()->GetAnimInstance());
}

void AIGIPlayerCharacter::RefreshInputMappingContext()
{
	auto* PlayerController = Cast<APlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (!IsValid(InputMappingContext))
	{
		UE_LOG(LogTemp, Error, TEXT("IGI input mapping context is invalid. ALS input assets were not loaded."));
		return;
	}

	auto* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return;
	}

	auto* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!IsValid(InputSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("IGI player could not access Enhanced Input Local Player Subsystem."));
		return;
	}

	InputSubsystem->RemoveMappingContext(InputMappingContext);

	FModifyContextOptions Options;
	Options.bNotifyUserSettings = true;
	InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
}

void AIGIPlayerCharacter::RemoveInputMappingContext(APlayerController* PlayerController) const
{
	if (!IsValid(PlayerController) || !IsValid(InputMappingContext))
	{
		return;
	}

	auto* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return;
	}

	if (auto* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		IsValid(InputSubsystem))
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
}
