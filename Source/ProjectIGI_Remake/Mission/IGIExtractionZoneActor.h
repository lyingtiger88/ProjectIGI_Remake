#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IGIExtractionZoneActor.generated.h"

class USphereComponent;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Extraction Zone"))
class PROJECTIGI_REMAKE_API AIGIExtractionZoneActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIExtractionZoneActor();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    TObjectPtr<USphereComponent> ExtractionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission", meta = (ClampMin = "100.0", ForceUnits = "cm"))
    float Radius = 700.0f;
};
