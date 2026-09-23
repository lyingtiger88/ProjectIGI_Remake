#include "Environment/IGIWeatherWorldSubsystem.h"

void UIGIWeatherWorldSubsystem::SetWeatherState(const FIGIWeatherState& NewWeatherState)
{
    CurrentWeatherState = NewWeatherState;
    CurrentWeatherState.PrecipitationIntensity =
        FMath::Clamp(CurrentWeatherState.PrecipitationIntensity, 0.0f, 1.0f);
    CurrentWeatherState.SurfaceWetness =
        FMath::Clamp(CurrentWeatherState.SurfaceWetness, 0.0f, 1.0f);
    CurrentWeatherState.SnowDepthFactor =
        FMath::Clamp(CurrentWeatherState.SnowDepthFactor, 0.0f, 1.0f);
    CurrentWeatherState.WindSpeedCmPerSecond =
        FMath::Max(0.0f, CurrentWeatherState.WindSpeedCmPerSecond);

    if (!CurrentWeatherState.WindDirection.IsNearlyZero())
    {
        CurrentWeatherState.WindDirection.Normalize();
    }
    else
    {
        CurrentWeatherState.WindDirection = FVector(1.0f, 0.0f, 0.0f);
    }

    OnWeatherChanged.Broadcast(CurrentWeatherState);
}

FVector UIGIWeatherWorldSubsystem::GetWindVelocity() const
{
    return CurrentWeatherState.WindDirection.GetSafeNormal() *
        CurrentWeatherState.WindSpeedCmPerSecond;
}
