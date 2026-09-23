#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Vision/IGIVisionTypes.h"
#include "IGIVisionComponent.generated.h"

class UCameraComponent;
class UIGIInventoryComponent;
class UMaterialInterface;
class UPostProcessComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FIGIVisionModeChangedSignature,
    EIGIVisionMode,
    PreviousMode,
    EIGIVisionMode,
    NewMode);

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGIVisionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UIGIVisionComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    void InitializeVision(UCameraComponent* InCamera, UIGIInventoryComponent* InInventory);

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    bool SetVisionMode(EIGIVisionMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    bool ToggleBinoculars();

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    bool ToggleNightVision();

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    bool ToggleThermal();

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    bool AdjustBinocularZoom(float FovDeltaDegrees);

    UFUNCTION(BlueprintCallable, Category = "IGI|Vision")
    void DisableVision();

    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    EIGIVisionMode GetVisionMode() const { return ActiveMode; }

    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    bool IsBinocularsActive() const { return ActiveMode == EIGIVisionMode::Binoculars; }

    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    bool IsNightVisionActive() const { return ActiveMode == EIGIVisionMode::NightVision; }

    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    bool IsThermalActive() const { return ActiveMode == EIGIVisionMode::Thermal; }

    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    bool HasRequiredEquipment(EIGIVisionMode Mode) const;

    // Returns zero when the current mode should keep the normal gameplay camera FOV.
    UFUNCTION(BlueprintPure, Category = "IGI|Vision")
    float GetFieldOfViewOverride() const;

    UPROPERTY(BlueprintAssignable, Category = "IGI|Vision")
    FIGIVisionModeChangedSignature OnVisionModeChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Requirements")
    bool bRequireOwnedEquipment = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Binoculars", meta = (ClampMin = "5.0", ClampMax = "80.0"))
    float BinocularDefaultFov = 28.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Binoculars", meta = (ClampMin = "3.0", ClampMax = "60.0"))
    float BinocularMinimumFov = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Binoculars", meta = (ClampMin = "10.0", ClampMax = "90.0"))
    float BinocularMaximumFov = 48.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|Binoculars", meta = (ClampMin = "1.0", ClampMax = "20.0"))
    float BinocularZoomStep = 4.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess")
    TSoftObjectPtr<UMaterialInterface> BinocularPostProcessMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess")
    TSoftObjectPtr<UMaterialInterface> NightVisionPostProcessMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess")
    TSoftObjectPtr<UMaterialInterface> ThermalPostProcessMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BinocularPostProcessWeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float NightVisionPostProcessWeight = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Vision|PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ThermalPostProcessWeight = 1.0f;

private:
    UPROPERTY(Transient)
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(Transient)
    TObjectPtr<UIGIInventoryComponent> Inventory;

    UPROPERTY(Transient)
    TObjectPtr<UPostProcessComponent> VisionPostProcess;

    UPROPERTY(Transient)
    EIGIVisionMode ActiveMode = EIGIVisionMode::Normal;

    float CurrentBinocularFov = 28.0f;

    void RefreshPostProcess();
    UMaterialInterface* ResolveMaterialForMode(EIGIVisionMode Mode) const;
    float GetPostProcessWeightForMode(EIGIVisionMode Mode) const;
};
