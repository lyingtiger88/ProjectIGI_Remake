#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IGIHUD.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI HUD"))
class PROJECTIGI_REMAKE_API AIGIHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|HUD")
    float ScreenMargin = 28.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|HUD")
    float HealthBarWidth = 220.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|HUD")
    float HealthBarHeight = 14.0f;

private:
    void DrawMissionStatus(float X, float Y);
    void DrawPlayerStatus(float X, float Y);
    void DrawWeaponStatus(float X, float Y);
};
