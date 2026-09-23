#include "IGIEnemyAIController.h"

#include "IGIEnemyCharacter.h"
#include "IGIPlayerCharacter.h"

bool AIGIEnemyAIController::BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const
{
	if (!Super::BDFR_ShouldProcessPerceivedActor_Implementation(SourceActor))
	{
		return false;
	}

	return IsValid(Cast<AIGIPlayerCharacter>(SourceActor));
}

bool AIGIEnemyAIController::BDFR_ShouldRespondToDistress_Implementation(AActor* SourceActor) const
{
	if (!Super::BDFR_ShouldRespondToDistress_Implementation(SourceActor))
	{
		return false;
	}

	return IsValid(Cast<AIGIEnemyCharacter>(SourceActor));
}
