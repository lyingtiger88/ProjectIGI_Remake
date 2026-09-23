#include "AI/IGIDogAIController.h"

#include "IGIPlayerCharacter.h"

bool AIGIDogAIController::BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const
{
	return IsValid(SourceActor) && SourceActor->IsA<AIGIPlayerCharacter>();
}
