#pragma once

#include "CoreMinimal.h"
#include "IGIEnemyTacticalTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIEnemyTacticalState : uint8
{
    Idle        UMETA(DisplayName = "Idle / Patrol"),
    Investigate UMETA(DisplayName = "Investigate"),
    Search      UMETA(DisplayName = "Search"),
    TakeCover   UMETA(DisplayName = "Take Cover"),
    Combat      UMETA(DisplayName = "Combat"),
    Dead        UMETA(DisplayName = "Dead")
};
