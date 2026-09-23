#pragma once

#include "CoreMinimal.h"
#include "IGIDistractionTypes.generated.h"

USTRUCT(BlueprintType)
struct PROJECTIGI_REMAKE_API FIGIDistractionDifficultyTuning
{
    GENERATED_BODY()

    // Effective acoustic strength required before this tier will even consider the lure.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinimumStrength = 0.15f;

    // Final response-score threshold after distance, alertness and repeat-use penalties.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ResponseThreshold = 0.20f;

    // Extra uncertainty after the score check. Higher tiers intentionally get much lower values.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ResponseChance = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "100.0", ForceUnits = "cm"))
    float MaxInvestigationDistance = 3200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "0.25", ForceUnits = "s"))
    float InvestigationSeconds = 9.0f;

    // Applied for repeated lures near the same place within the memory window.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RepeatPenaltyPerUse = 0.10f;

    // On harder tiers, an alerted guard refuses low-value diversion attempts entirely.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IGI|Distraction")
    bool bIgnoreWhenAlerted = false;
};
