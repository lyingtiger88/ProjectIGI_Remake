#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment/IGIWeatherTypes.h"
#include "IGIWeatherStateActor.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Weather State"))
class PROJECTIGI_REMAKE_API AIGIWeatherStateActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIWeatherStateActor();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Weather")
    void ApplyWeather();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather")
    FIGIWeatherState WeatherState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Weather")
    bool bApplyOnBeginPlay = true;
};
