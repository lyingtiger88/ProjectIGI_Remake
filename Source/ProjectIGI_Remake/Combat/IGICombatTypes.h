#pragma once

#include "CoreMinimal.h"
#include "IGICombatTypes.generated.h"

UENUM(BlueprintType)
enum class EIGICombatState : uint8
{
    Unarmed   UMETA(DisplayName = "Unarmed"),
    Armed     UMETA(DisplayName = "Armed"),
    Aiming    UMETA(DisplayName = "Aiming"),
    Firing    UMETA(DisplayName = "Firing"),
    Reloading UMETA(DisplayName = "Reloading"),
    Melee     UMETA(DisplayName = "Melee"),
    Takedown  UMETA(DisplayName = "Takedown")
};
