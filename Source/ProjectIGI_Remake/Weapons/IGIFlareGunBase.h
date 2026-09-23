#pragma once

#include "CoreMinimal.h"
#include "Weapons/IGIFirearmBase.h"
#include "IGIFlareGunBase.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Flare Gun"))
class PROJECTIGI_REMAKE_API AIGIFlareGunBase : public AIGIFirearmBase
{
    GENERATED_BODY()

public:
    AIGIFlareGunBase();

    virtual bool FireHitscan(AController* InstigatorController) override;
};
