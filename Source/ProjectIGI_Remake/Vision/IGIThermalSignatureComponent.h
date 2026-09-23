#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IGIThermalSignatureComponent.generated.h"

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIThermalSignatureComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIThermalSignatureComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision|Thermal")
    void SetThermalSignatureEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "IGI|Vision|Thermal")
    bool IsThermalSignatureEnabled() const { return bThermalSignatureEnabled; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Thermal")
    bool bEnableOnBeginPlay = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Thermal", meta = (ClampMin = "1", ClampMax = "255"))
    int32 CustomDepthStencilValue = 246;

private:
    bool bThermalSignatureEnabled = false;
};
