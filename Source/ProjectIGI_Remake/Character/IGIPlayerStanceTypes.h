#pragma once

#include "CoreMinimal.h"
#include "IGIPlayerStanceTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIPlayerStance : uint8
{
    Standing   UMETA(DisplayName = "Standing"),
    Crouching  UMETA(DisplayName = "Crouching"),
    Prone      UMETA(DisplayName = "Prone")
};
