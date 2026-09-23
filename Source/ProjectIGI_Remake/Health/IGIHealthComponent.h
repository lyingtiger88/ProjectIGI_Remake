#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGIHealthComponent.generated.h"

class AController;
class UDamageType;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FIGIHealthChangedSignature,
    float,
    CurrentHealth,
    float,
    MaxHealth,
    float,
    Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FIGIDeathSignature,
    AActor*,
    DeadActor,
    AActor*,
    DamageCauser);

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIHealthComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category = "IGI|Health")
    float GetHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "IGI|Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "IGI|Health")
    float GetHealthNormalized() const;

    UFUNCTION(BlueprintPure, Category = "IGI|Health")
    bool IsAlive() const { return CurrentHealth > 0.0f; }

    UFUNCTION(BlueprintPure, Category = "IGI|Health")
    bool IsFullHealth() const { return CurrentHealth >= MaxHealth - KINDA_SMALL_NUMBER; }

    UFUNCTION(BlueprintCallable, Category = "IGI|Health")
    float Heal(float Amount);

    UFUNCTION(BlueprintCallable, Category = "IGI|Health")
    float ApplyDamage(float Amount, AActor* DamageCauser = nullptr);

    UFUNCTION(BlueprintCallable, Category = "IGI|Health")
    void SetHealth(float NewHealth);

    UPROPERTY(BlueprintAssignable, Category = "IGI|Health")
    FIGIHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Health")
    FIGIDeathSignature OnDeath;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Health")
    bool bStartAtFullHealth = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Health", meta = (ClampMin = "0.0"))
    float InitialHealth = 100.0f;

private:
    UPROPERTY(Transient)
    float CurrentHealth = 100.0f;

    bool bDeathBroadcast = false;

    UFUNCTION()
    void HandleOwnerTakeAnyDamage(
        AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* InstigatedBy,
        AActor* DamageCauser);
};
