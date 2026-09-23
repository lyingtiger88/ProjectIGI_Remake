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

UENUM(BlueprintType)
enum class EIGIProneOrientation : uint8
{
    ChestDown UMETA(DisplayName = "Chest Down"),
    Supine    UMETA(DisplayName = "Supine / On Back")
};

UENUM(BlueprintType)
enum class EIGIProneRollDirection : uint8
{
    Left  UMETA(DisplayName = "Left"),
    Right UMETA(DisplayName = "Right")
};
