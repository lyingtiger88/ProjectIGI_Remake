#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Environment/IGIWeatherTypes.h"
#include "IGIWeatherWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIGIWeatherChangedSignature, const FIGIWeatherState&, WeatherState);

UCLASS(BlueprintType)
class PROJECTIGI_REMAKE_API UIGIWeatherWorldSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "IGI|Weather")
    void SetWeatherState(const FIGIWeatherState& NewWeatherState);

    UFUNCTION(BlueprintPure, Category = "IGI|Weather")
    const FIGIWeatherState& GetWeatherState() const { return CurrentWeatherState; }

    UFUNCTION(BlueprintPure, Category = "IGI|Weather")
    FVector GetWindVelocity() const;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Weather")
    FIGIWeatherChangedSignature OnWeatherChanged;

private:
    UPROPERTY(Transient)
    FIGIWeatherState CurrentWeatherState;
};
