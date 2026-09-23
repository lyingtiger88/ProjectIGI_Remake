#pragma once

#include "CoreMinimal.h"
#include "IGIVisionTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIVisionMode : uint8
{
    Normal      UMETA(DisplayName = "Normal"),
    Binoculars  UMETA(DisplayName = "Binoculars"),
    NightVision UMETA(DisplayName = "Night Vision"),
    Thermal     UMETA(DisplayName = "Thermal / Infrared")
};
