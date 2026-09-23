#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IGIGameModeBase.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Game Mode"))
class PROJECTIGI_REMAKE_API AIGIGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIGIGameModeBase();
};
