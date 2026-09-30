#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGIHitReactionComponent.generated.h"

class AController;
class UDamageType;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(
    FIGIHitReactionSignature,
    AActor*,
    HitActor,
    float,
    Damage,
    FVector,
    HitLocation,
    FVector,
    ShotDirection,
    FName,
    BoneName,
    AActor*,
    DamageCauser);

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIHitReactionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIHitReactionComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category = "IGI|Damage|Reaction")
    FVector GetLastHitLocation() const { return LastHitLocation; }

    UFUNCTION(BlueprintPure, Category = "IGI|Damage|Reaction")
    FVector GetLastShotDirection() const { return LastShotDirection; }

    UFUNCTION(BlueprintPure, Category = "IGI|Damage|Reaction")
    FName GetLastHitBone() const { return LastHitBone; }

    UFUNCTION(BlueprintPure, Category = "IGI|Damage|Reaction")
    float GetLastHitDamage() const { return LastHitDamage; }

    UPROPERTY(BlueprintAssignable, Category = "IGI|Damage|Reaction")
    FIGIHitReactionSignature OnHitReaction;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Damage|Reaction")
    bool bApplyMovementImpulse = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Damage|Reaction", meta = (ClampMin = "0.0"))
    float HorizontalImpulse = 110.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Damage|Reaction", meta = (ClampMin = "0.0"))
    float UpwardImpulse = 18.0f;

private:
    UPROPERTY(Transient)
    FVector LastHitLocation = FVector::ZeroVector;

    UPROPERTY(Transient)
    FVector LastShotDirection = FVector::ZeroVector;

    UPROPERTY(Transient)
    FName LastHitBone = NAME_None;

    UPROPERTY(Transient)
    float LastHitDamage = 0.0f;

    UFUNCTION()
    void HandleOwnerTakePointDamage(
        AActor* DamagedActor,
        float Damage,
        AController* InstigatedBy,
        FVector HitLocation,
        UPrimitiveComponent* HitComponent,
        FName BoneName,
        FVector ShotFromDirection,
        const UDamageType* DamageType,
        AActor* DamageCauser);
};
