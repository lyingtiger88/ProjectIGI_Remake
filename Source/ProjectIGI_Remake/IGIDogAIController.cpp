#include "IGIDogAIController.h"

#include "IGIPlayerCharacter.h"

bool AIGIDogAIController::BDFR_ShouldProcessPerceivedActor_Implementation(
    AActor* SourceActor) const
{
    if (!Super::BDFR_ShouldProcessPerceivedActor_Implementation(SourceActor))
    {
        return false;
    }

    return IsValid(Cast<AIGIPlayerCharacter>(SourceActor));
}
