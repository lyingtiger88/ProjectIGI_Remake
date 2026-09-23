#include "Environment/IGIWeatherStateActor.h"

#include "Environment/IGIWeatherWorldSubsystem.h"
#include "Engine/World.h"

AIGIWeatherStateActor::AIGIWeatherStateActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AIGIWeatherStateActor::BeginPlay()
{
    Super::BeginPlay();

    if (bApplyOnBeginPlay)
    {
        ApplyWeather();
    }
}

void AIGIWeatherStateActor::ApplyWeather()
{
    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        return;
    }

    if (UIGIWeatherWorldSubsystem* Weather =
            World->GetSubsystem<UIGIWeatherWorldSubsystem>();
        IsValid(Weather))
    {
        Weather->SetWeatherState(WeatherState);
    }
}
