#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/IGIWeaponTypes.h"
#include "IGIVerticalSliceMissionDirectorActor.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Vertical Slice Mission Director"))
class PROJECTIGI_REMAKE_API AIGIVerticalSliceMissionDirectorActor : public AActor
{
    GENERATED_BODY()

public:
    AIGIVerticalSliceMissionDirectorActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    FName PrimaryObjectiveId = TEXT("PrimaryIntel");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    EIGIFlarePurpose ExtractionFlarePurpose = EIGIFlarePurpose::RescueExtraction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Mission")
    bool bAutoStartMission = true;
};
