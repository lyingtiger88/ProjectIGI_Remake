#include "IGIDogCharacter.h"

#include "IGIDogAIController.h"

AIGIDogCharacter::AIGIDogCharacter()
{
    AIControllerClass = AIGIDogAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AIGIDogCharacter::BDFR_UpdateCanineAttention_Implementation(
    const FBDFRCanineAttentionSnapshot& Snapshot)
{
    CurrentAttentionSnapshot = Snapshot;
    IGI_OnCanineAttentionUpdated(Snapshot);
}

void AIGIDogCharacter::BDFR_PlayCanineEngage_Implementation(
    AActor* TargetActor)
{
    IGI_OnCanineEngageRequested(TargetActor);
}

void AIGIDogCharacter::BDFR_PlayCanineBark_Implementation(
    const FName ReasonTag)
{
    IGI_OnCanineBarkRequested(ReasonTag);
}
