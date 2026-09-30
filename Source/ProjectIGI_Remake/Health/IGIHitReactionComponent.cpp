#include "Health/IGIHitReactionComponent.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UIGIHitReactionComponent::UIGIHitReactionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGIHitReactionComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner(); IsValid(Owner))
    {
        Owner->OnTakePointDamage.AddDynamic(
            this,
            &ThisClass::HandleOwnerTakePointDamage);
    }
}

void UIGIHitReactionComponent::HandleOwnerTakePointDamage(
    AActor* DamagedActor,
    const float Damage,
    AController* InstigatedBy,
    const FVector HitLocation,
    UPrimitiveComponent* HitComponent,
    const FName BoneName,
    const FVector ShotFromDirection,
    const UDamageType* DamageType,
    AActor* DamageCauser)
{
    if (!IsValid(DamagedActor) || Damage <= 0.0f)
    {
        return;
    }

    LastHitLocation = HitLocation;
    LastShotDirection = ShotFromDirection.GetSafeNormal();
    LastHitBone = BoneName;
    LastHitDamage = Damage;

    if (bApplyMovementImpulse)
    {
        if (ACharacter* Character = Cast<ACharacter>(DamagedActor);
            IsValid(Character) && IsValid(Character->GetCharacterMovement()))
        {
            const FVector ReactionImpulse =
                LastShotDirection * HorizontalImpulse +
                FVector::UpVector * UpwardImpulse;

            Character->GetCharacterMovement()->AddImpulse(
                ReactionImpulse,
                true);
        }
    }

    OnHitReaction.Broadcast(
        DamagedActor,
        Damage,
        HitLocation,
        LastShotDirection,
        BoneName,
        DamageCauser);
}
