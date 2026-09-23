#include "Health/IGIHealthComponent.h"

#include "GameFramework/Actor.h"

UIGIHealthComponent::UIGIHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGIHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    MaxHealth = FMath::Max(1.0f, MaxHealth);
    CurrentHealth = bStartAtFullHealth
        ? MaxHealth
        : FMath::Clamp(InitialHealth, 0.0f, MaxHealth);

    if (AActor* Owner = GetOwner(); IsValid(Owner))
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &ThisClass::HandleOwnerTakeAnyDamage);
    }

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, 0.0f);
}

float UIGIHealthComponent::GetHealthNormalized() const
{
    return MaxHealth > KINDA_SMALL_NUMBER
        ? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f)
        : 0.0f;
}

float UIGIHealthComponent::Heal(const float Amount)
{
    if (Amount <= 0.0f || !IsAlive() || IsFullHealth())
    {
        return 0.0f;
    }

    const float Previous = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
    const float Applied = CurrentHealth - Previous;

    if (Applied > KINDA_SMALL_NUMBER)
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, Applied);
    }

    return Applied;
}

float UIGIHealthComponent::ApplyDamage(const float Amount, AActor* DamageCauser)
{
    if (Amount <= 0.0f || !IsAlive())
    {
        return 0.0f;
    }

    const float Previous = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
    const float Applied = Previous - CurrentHealth;

    if (Applied > KINDA_SMALL_NUMBER)
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, -Applied);
    }

    if (CurrentHealth <= 0.0f && !bDeathBroadcast)
    {
        bDeathBroadcast = true;
        OnDeath.Broadcast(GetOwner(), DamageCauser);
    }

    return Applied;
}

void UIGIHealthComponent::SetHealth(const float NewHealth)
{
    const float Previous = CurrentHealth;
    CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const float Delta = CurrentHealth - Previous;

    if (!FMath::IsNearlyZero(Delta))
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth, Delta);
    }

    if (CurrentHealth > 0.0f)
    {
        bDeathBroadcast = false;
    }
    else if (!bDeathBroadcast)
    {
        bDeathBroadcast = true;
        OnDeath.Broadcast(GetOwner(), nullptr);
    }
}

void UIGIHealthComponent::HandleOwnerTakeAnyDamage(
    AActor* DamagedActor,
    const float Damage,
    const UDamageType* DamageType,
    AController* InstigatedBy,
    AActor* DamageCauser)
{
    ApplyDamage(Damage, DamageCauser);
}
