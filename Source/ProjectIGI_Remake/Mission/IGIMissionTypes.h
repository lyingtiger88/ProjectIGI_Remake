#pragma once

#include "CoreMinimal.h"
#include "IGIMissionTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIMissionState : uint8
{
    Inactive         UMETA(DisplayName = "Inactive"),
    PrimaryObjective UMETA(DisplayName = "Primary Objective"),
    Extraction       UMETA(DisplayName = "Extraction"),
    Completed        UMETA(DisplayName = "Completed"),
    Failed           UMETA(DisplayName = "Failed")
};
