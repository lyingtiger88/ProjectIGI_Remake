#pragma once

#include "CoreMinimal.h"
#include "Behavior/BDFRAIController.h"
#include "IGIEnemyAIController.generated.h"

UCLASS(Blueprintable)
class PROJECTIGI_REMAKE_API AIGIEnemyAIController : public ABDFRAIController
{
	GENERATED_BODY()

protected:
	virtual bool BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const override;
};
