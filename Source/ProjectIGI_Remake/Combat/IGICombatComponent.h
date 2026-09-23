#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/IGICombatTypes.h"
#include "IGICombatComponent.generated.h"

class AIGIWeaponBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FIGICombatStateChangedSignature, EIGICombatState, PreviousState, EIGICombatState, NewState);

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGICombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGICombatComponent();

    UFUNCTION(BlueprintPure, Category = "IGI|Combat")
    EIGICombatState GetCombatState() const { return CombatState; }

    UFUNCTION(BlueprintPure, Category = "IGI|Combat")
    AIGIWeaponBase* GetActiveWeapon() const { return ActiveWeapon; }

    UFUNCTION(BlueprintPure, Category = "IGI|Combat")
    bool IsAiming() const { return bIsAiming; }

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    void SetActiveWeapon(AIGIWeaponBase* NewWeapon);

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    void SetCombatState(EIGICombatState NewState);

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    void StartAim();

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    void StopAim();

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    bool BeginReload();

    UFUNCTION(BlueprintCallable, Category = "IGI|Combat")
    void EndReload();

    UPROPERTY(BlueprintAssignable, Category = "IGI|Combat")
    FIGICombatStateChangedSignature OnCombatStateChanged;

private:
    UPROPERTY(Transient)
    TObjectPtr<AIGIWeaponBase> ActiveWeapon;

    UPROPERTY(Transient)
    EIGICombatState CombatState = EIGICombatState::Unarmed;

    UPROPERTY(Transient)
    bool bIsAiming = false;
};
