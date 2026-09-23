#pragma once

#include "CoreMinimal.h"
#include "IGIWeatherTypes.generated.h"

UENUM(BlueprintType)
enum class EIGIWeatherType : uint8
{
    Clear      UMETA(DisplayName = "Clear"),
    Rain       UMETA(DisplayName = "Rain"),
    Snow       UMETA(DisplayName = "Snow"),
    Storm      UMETA(DisplayName = "Storm"),
    Sandstorm  UMETA(DisplayName = "Sandstorm")
};

USTRUCT(BlueprintType)
struct PROJECTIGI_REMAKE_API FIGIWeatherState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather")
    EIGIWeatherType WeatherType = EIGIWeatherType::Clear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float PrecipitationIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather")
    FVector WindDirection = FVector(1.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather", meta = (ClampMin = "0.0", ForceUnits = "cm/s"))
    float WindSpeedCmPerSecond = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SurfaceWetness = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float SnowDepthFactor = 0.0f;
};
