#include "IGIPlayerCharacter.h"

#include "AlsAnimationInstance.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "IndirectVisual/BDFRIndirectVisualSourceComponent.h"
#include "Tracking/BDFRTrackEmitterComponent.h"
#include "Math/RotationMatrix.h"
#include "Settings/AlsCharacterSettings.h"
#include "Settings/AlsMovementSettings.h"
#include "UObject/ConstructorHelpers.h"

AIGIPlayerCharacter::AIGIPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 45.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 14.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	IndirectVisualSourceComponent =
		CreateDefaultSubobject<UBDFRIndirectVisualSourceComponent>(TEXT("BDFRIndirectVisualSource"));
	TrackEmitterComponent =
		CreateDefaultSubobject<UBDFRTrackEmitterComponent>(TEXT("BDFRTrackEmitter"));

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
	const auto* PreviousPlayer = Cast<APlayerController>(PreviousController);
	if (IsValid(PreviousPlayer) && IsValid(InputMappingContext))
	{
		if (auto* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer());
			IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer = Cast<APlayerController>(GetController());
	if (IsValid(NewPlayer) && IsValid(InputMappingContext))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		if (auto* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer());
			IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;
			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
		}
	}

	Super::NotifyControllerChanged();
}

void AIGIPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInput)) { return; }

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
	if (IsValid(WalkAction)) { EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk); }
	if (IsValid(CrouchAction)) { EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch); }
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
	if (!IsValid(GetController())) { return; }
	const FVector2D Value = ActionValue.Get<FVector2D>();
	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0.0, ControlRotation.Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection, Value.Y);
	AddMovementInput(RightDirection, Value.X);
}

void AIGIPlayerCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
}

void AIGIPlayerCharacter::Input_OnWalk()
{
	SetDesiredGait(GetDesiredGait() == AlsGaitTags::Walking ? AlsGaitTags::Running : AlsGaitTags::Walking);
}

void AIGIPlayerCharacter::Input_OnCrouch()
{
	SetDesiredStance(GetDesiredStance() == AlsStanceTags::Crouching ? AlsStanceTags::Standing : AlsStanceTags::Crouching);
}

void AIGIPlayerCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}
		Jump();
	}
	else { StopJumping(); }
}

void AIGIPlayerCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void AIGIPlayerCharacter::RefreshAlsAnimationInstance()
{
	AnimationInstance = Cast<UAlsAnimationInstance>(GetMesh()->GetAnimInstance());
}
