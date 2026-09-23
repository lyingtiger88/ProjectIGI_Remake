#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "Character/IGIPlayerStanceTypes.h"
#include "TimerManager.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIPlayerCharacter.generated.h"

class APlayerController;
class UCameraComponent;
class UBDFRTrackEmitterComponent;
class UIGIAcousticSignatureComponent;
class UIGICombatComponent;
class UIGIInventoryComponent;
class UIGITrackingSurfaceComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FIGIPlayerStanceChangedSignature,
	EIGIPlayerStance,
	PreviousStance,
	EIGIPlayerStance,
	NewStance);

UCLASS()
class PROJECTIGI_REMAKE_API AIGIPlayerCharacter : public AAlsCharacter
{
	GENERATED_BODY()

public:
	AIGIPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void NotifyControllerChanged() override;
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintPure, Category = "IGI|Tracking")
	UBDFRTrackEmitterComponent* GetTrackEmitterComponent() const { return TrackEmitterComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|Tracking")
	UIGITrackingSurfaceComponent* GetTrackingSurfaceComponent() const { return TrackingSurfaceComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|Combat")
	UIGICombatComponent* GetCombatComponent() const { return CombatComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|Inventory")
	UIGIInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|Stealth")
	UIGIAcousticSignatureComponent* GetAcousticSignatureComponent() const { return AcousticSignatureComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|Stance")
	EIGIPlayerStance GetPlayerStance() const { return PlayerStance; }

	UFUNCTION(BlueprintPure, Category = "IGI|Stance")
	bool IsProne() const { return PlayerStance == EIGIPlayerStance::Prone; }

	UFUNCTION(BlueprintPure, Category = "IGI|Stance")
	bool IsProneAiming() const { return IsProne() && bAimInputHeld; }

	UFUNCTION(BlueprintPure, Category = "IGI|Stance")
	float GetProneMovementDirectionAngle() const;

	UFUNCTION(BlueprintPure, Category = "IGI|Stance")
	float GetProneNormalizedSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "IGI|Stance")
	bool SetPlayerStance(EIGIPlayerStance NewStance);

	UFUNCTION(BlueprintPure, Category = "IGI|Camera")
	bool IsRightShoulderCamera() const { return bRightShoulderCamera; }

	UPROPERTY(BlueprintAssignable, Category = "IGI|Stance")
	FIGIPlayerStanceChangedSignature OnPlayerStanceChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Tracking")
	TObjectPtr<UBDFRTrackEmitterComponent> TrackEmitterComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Tracking")
	TObjectPtr<UIGITrackingSurfaceComponent> TrackingSurfaceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Combat")
	TObjectPtr<UIGICombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Inventory")
	TObjectPtr<UIGIInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Stealth")
	TObjectPtr<UIGIAcousticSignatureComponent> AcousticSignatureComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "IGI|Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stance", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float HoldCrouchToProneSeconds = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stance", meta = (ClampMin = "30.0"))
	float ProneCapsuleHalfHeight = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stance", meta = (ClampMin = "30.0"))
	float CrouchedCapsuleHalfHeight = 56.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Stance", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float ProneMovementInputScale = 0.36f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	float CameraTransitionSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	FVector StandingCameraOffset = FVector(0.0f, 45.0f, 65.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	FVector CrouchingCameraOffset = FVector(0.0f, 45.0f, 47.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	FVector ProneCameraOffset = FVector(0.0f, 38.0f, 19.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	FVector StandingAimCameraOffset = FVector(0.0f, 60.0f, 61.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	FVector CrouchingAimCameraOffset = FVector(0.0f, 58.0f, 44.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	FVector ProneAimCameraOffset = FVector(0.0f, 48.0f, 16.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	float HipCameraArmLength = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	float ProneHipCameraArmLength = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	float AimCameraArmLength = 225.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	float ProneAimCameraArmLength = 190.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera")
	float HipFieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	float AimFieldOfView = 76.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Camera|Aim")
	float ProneAimFieldOfView = 72.0f;

protected:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);
	void Input_OnLook(const FInputActionValue& ActionValue);
	void Input_OnMove(const FInputActionValue& ActionValue);
	void Input_OnSprint(const FInputActionValue& ActionValue);
	void Input_OnWalk();
	void Input_OnStancePressed();
	void Input_OnStanceReleased();
	void Input_OnJump(const FInputActionValue& ActionValue);
	void Input_OnAim(const FInputActionValue& ActionValue);
	void Input_OnFire();
	void Input_OnReload();
	void Input_OnEquipWeapon01();
	void Input_OnEquipWeapon02();
	void Input_OnEquipWeapon03();
	void Input_OnEquipWeapon04();
	void Input_OnEquipKnife();
	void Input_OnSwitchShoulder();

private:
	UPROPERTY(Transient)
	EIGIPlayerStance PlayerStance = EIGIPlayerStance::Standing;

	bool bAimInputHeld = false;
	bool bRightShoulderCamera = true;
	bool bStanceHoldTriggered = false;

	float StandingCapsuleHalfHeight = 88.0f;

	FTimerHandle StanceHoldTimer;

	void EquipInventorySlot(EIGICarrySlot Slot);
	void TriggerProneFromStanceHold();
	bool CanExpandCapsuleTo(float TargetHalfHeight) const;
	bool ResizeCapsuleKeepingFeet(float TargetHalfHeight);
	void RefreshCameraPresentation(float DeltaSeconds);
	FVector GetTargetCameraOffset() const;
	float GetTargetCameraArmLength() const;
	float GetTargetCameraFieldOfView() const;
	void RefreshAlsAnimationInstance();
	void RefreshInputMappingContext();
	void RemoveInputMappingContext(APlayerController* PlayerController) const;
};
