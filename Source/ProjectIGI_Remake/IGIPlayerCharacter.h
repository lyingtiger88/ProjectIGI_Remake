#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
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

UCLASS()
class PROJECTIGI_REMAKE_API AIGIPlayerCharacter : public AAlsCharacter
{
	GENERATED_BODY()

public:
	AIGIPlayerCharacter();

	virtual void BeginPlay() override;
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

protected:
	void Input_OnLookMouse(const FInputActionValue& ActionValue);
	void Input_OnLook(const FInputActionValue& ActionValue);
	void Input_OnMove(const FInputActionValue& ActionValue);
	void Input_OnSprint(const FInputActionValue& ActionValue);
	void Input_OnWalk();
	void Input_OnCrouch();
	void Input_OnJump(const FInputActionValue& ActionValue);
	void Input_OnAim(const FInputActionValue& ActionValue);

private:
	void RefreshAlsAnimationInstance();
	void RefreshInputMappingContext();
	void RemoveInputMappingContext(APlayerController* PlayerController) const;
};
