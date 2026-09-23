#pragma once

#include "CoreMinimal.h"
#include "Behavior/BDFRCanineAIController.h"
#include "IGIDogAIController.generated.h"

UCLASS()
class PROJECTIGI_REMAKE_API AIGIDogAIController : public ABDFRCanineAIController
{
    GENERATED_BODY()

protected:
    virtual bool BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const override;
};
