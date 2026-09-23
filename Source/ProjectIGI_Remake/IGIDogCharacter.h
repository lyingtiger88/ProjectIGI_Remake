#pragma once

#include "CoreMinimal.h"
#include "Canine/BDFRCanineTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/BDFRCaninePresentationInterface.h"
#include "IGIDogCharacter.generated.h"

UCLASS(Blueprintable)
class PROJECTIGI_REMAKE_API AIGIDogCharacter
    : public ACharacter
    , public IBDFRCaninePresentationInterface
{
    GENERATED_BODY()

public:
    AIGIDogCharacter();

    UFUNCTION(BlueprintPure, Category = "IGI|Dog|Attention")
    FBDFRCanineAttentionSnapshot GetCanineAttentionSnapshot() const
    {
        return CurrentAttentionSnapshot;
    }

protected:
    virtual void BDFR_UpdateCanineAttention_Implementation(
        const FBDFRCanineAttentionSnapshot& Snapshot) override;

    virtual void BDFR_PlayCanineEngage_Implementation(
        AActor* TargetActor) override;

    virtual void BDFR_PlayCanineBark_Implementation(
        FName ReasonTag) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "IGI|Dog|Animation")
    void IGI_OnCanineAttentionUpdated(
        const FBDFRCanineAttentionSnapshot& Snapshot);

    UFUNCTION(BlueprintImplementableEvent, Category = "IGI|Dog|Animation")
    void IGI_OnCanineEngageRequested(AActor* TargetActor);

    UFUNCTION(BlueprintImplementableEvent, Category = "IGI|Dog|Animation")
    void IGI_OnCanineBarkRequested(FName ReasonTag);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Dog|Attention")
    FBDFRCanineAttentionSnapshot CurrentAttentionSnapshot;
};
