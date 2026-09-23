#include "IGIEnemyAIController.h"

#include "IGIPlayerCharacter.h"

bool AIGIEnemyAIController::BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const
{
	if (!Super::BDFR_ShouldProcessPerceivedActor_Implementation(SourceActor))
	{
		return false;
	}

	return IsValid(Cast<AIGIPlayerCharacter>(SourceActor));
}
