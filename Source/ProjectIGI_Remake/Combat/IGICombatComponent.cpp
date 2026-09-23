#include "Combat/IGICombatComponent.h"

#include "Weapons/IGIWeaponBase.h"

UIGICombatComponent::UIGICombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGICombatComponent::SetActiveWeapon(AIGIWeaponBase* NewWeapon)
{
    ActiveWeapon = NewWeapon;
    bIsAiming = false;
    SetCombatState(IsValid(ActiveWeapon) ? EIGICombatState::Armed : EIGICombatState::Unarmed);
}

void UIGICombatComponent::SetCombatState(const EIGICombatState NewState)
{
    if (CombatState == NewState)
    {
        return;
    }

    const EIGICombatState PreviousState = CombatState;
    CombatState = NewState;
    OnCombatStateChanged.Broadcast(PreviousState, CombatState);
}

void UIGICombatComponent::StartAim()
{
    if (!IsValid(ActiveWeapon) || CombatState == EIGICombatState::Reloading)
    {
        return;
    }

    bIsAiming = true;
    SetCombatState(EIGICombatState::Aiming);
}

void UIGICombatComponent::StopAim()
{
    bIsAiming = false;

    if (CombatState == EIGICombatState::Aiming)
    {
        SetCombatState(IsValid(ActiveWeapon) ? EIGICombatState::Armed : EIGICombatState::Unarmed);
    }
}

bool UIGICombatComponent::BeginReload()
{
    if (!IsValid(ActiveWeapon) || CombatState == EIGICombatState::Reloading)
    {
        return false;
    }

    bIsAiming = false;
    SetCombatState(EIGICombatState::Reloading);
    return true;
}

void UIGICombatComponent::EndReload()
{
    if (CombatState == EIGICombatState::Reloading)
    {
        SetCombatState(IsValid(ActiveWeapon) ? EIGICombatState::Armed : EIGICombatState::Unarmed);
    }
}
