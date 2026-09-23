#include "AI/IGIEnemyAIController.h"

#include "IGIPlayerCharacter.h"

bool AIGIEnemyAIController::BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const
{
	return IsValid(SourceActor) && SourceActor->IsA<AIGIPlayerCharacter>();
}
