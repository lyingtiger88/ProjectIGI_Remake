#pragma once

#include "CoreMinimal.h"
#include "Weapons/IGIFirearmBase.h"
#include "IGIFlareGunBase.generated.h"

class AIGIFlareProjectileActor;
class UNiagaraSystem;
class UStaticMesh;

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Flare Gun"))
class PROJECTIGI_REMAKE_API AIGIFlareGunBase : public AIGIFirearmBase
{
    GENERATED_BODY()

public:
    AIGIFlareGunBase();

    virtual bool FireHitscan(AController* InstigatorController) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    EIGIFlarePurpose FlarePurpose = EIGIFlarePurpose::Illumination;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TSubclassOf<AIGIFlareProjectileActor> FlareProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TSoftObjectPtr<UStaticMesh> FlareProjectileMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare")
    TSoftObjectPtr<UNiagaraSystem> FlareTrailEffect;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare", meta = (ClampMin = "100.0", ForceUnits = "cm/s"))
    float FlareLaunchSpeed = 1800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Flare", meta = (ClampMin = "1.0", ForceUnits = "s"))
    float FlareLifeSeconds = 22.0f;
};
