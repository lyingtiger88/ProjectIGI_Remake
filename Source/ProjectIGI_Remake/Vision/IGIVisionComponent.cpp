#include "Vision/IGIVisionComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/Actor.h"
#include "Inventory/IGIInventoryComponent.h"
#include "Materials/MaterialInterface.h"
#include "Weapons/IGIWeaponTypes.h"

UIGIVisionComponent::UIGIVisionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGIVisionComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentBinocularFov = FMath::Clamp(
        BinocularDefaultFov,
        BinocularMinimumFov,
        BinocularMaximumFov);

    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    VisionPostProcess = NewObject<UPostProcessComponent>(
        Owner,
        TEXT("IGIVisionPostProcess"));

    if (IsValid(VisionPostProcess))
    {
        Owner->AddInstanceComponent(VisionPostProcess);
        VisionPostProcess->bUnbound = true;
        VisionPostProcess->Priority = 1000.0f;
        VisionPostProcess->BlendWeight = 1.0f;
        VisionPostProcess->bEnabled = true;
        VisionPostProcess->RegisterComponent();
    }
}

void UIGIVisionComponent::InitializeVision(
    UCameraComponent* InCamera,
    UIGIInventoryComponent* InInventory)
{
    Camera = InCamera;
    Inventory = InInventory;
    RefreshPostProcess();
}

bool UIGIVisionComponent::SetVisionMode(const EIGIVisionMode NewMode)
{
    if (NewMode != EIGIVisionMode::Normal &&
        !HasRequiredEquipment(NewMode))
    {
        return false;
    }

    if (ActiveMode == NewMode)
    {
        return true;
    }

    const EIGIVisionMode PreviousMode = ActiveMode;
    ActiveMode = NewMode;

    if (ActiveMode == EIGIVisionMode::Binoculars)
    {
        CurrentBinocularFov = FMath::Clamp(
            CurrentBinocularFov,
            BinocularMinimumFov,
            BinocularMaximumFov);
    }

    RefreshPostProcess();
    OnVisionModeChanged.Broadcast(PreviousMode, ActiveMode);
    return true;
}

bool UIGIVisionComponent::ToggleBinoculars()
{
    return SetVisionMode(
        IsBinocularsActive()
            ? EIGIVisionMode::Normal
            : EIGIVisionMode::Binoculars);
}

bool UIGIVisionComponent::ToggleNightVision()
{
    return SetVisionMode(
        IsNightVisionActive()
            ? EIGIVisionMode::Normal
            : EIGIVisionMode::NightVision);
}

bool UIGIVisionComponent::ToggleThermal()
{
    return SetVisionMode(
        IsThermalActive()
            ? EIGIVisionMode::Normal
            : EIGIVisionMode::Thermal);
}

bool UIGIVisionComponent::AdjustBinocularZoom(const float FovDeltaDegrees)
{
    if (!IsBinocularsActive())
    {
        return false;
    }

    CurrentBinocularFov = FMath::Clamp(
        CurrentBinocularFov + FovDeltaDegrees,
        BinocularMinimumFov,
        BinocularMaximumFov);

    return true;
}

bool UIGIVisionComponent::ZoomBinocularsIn()
{
    return AdjustBinocularZoom(-FMath::Abs(BinocularZoomStep));
}

bool UIGIVisionComponent::ZoomBinocularsOut()
{
    return AdjustBinocularZoom(FMath::Abs(BinocularZoomStep));
}

void UIGIVisionComponent::DisableVision()
{
    if (ActiveMode != EIGIVisionMode::Normal)
    {
        SetVisionMode(EIGIVisionMode::Normal);
    }
}

bool UIGIVisionComponent::HasRequiredEquipment(const EIGIVisionMode Mode) const
{
    if (!bRequireOwnedEquipment || Mode == EIGIVisionMode::Normal)
    {
        return true;
    }

    if (!IsValid(Inventory))
    {
        return false;
    }

    switch (Mode)
    {
        case EIGIVisionMode::Binoculars:
            return Inventory->GetEquipmentCount(EIGIEquipmentType::Binoculars) > 0;

        case EIGIVisionMode::NightVision:
            return Inventory->GetEquipmentCount(EIGIEquipmentType::NightVisionGoggles) > 0;

        case EIGIVisionMode::Thermal:
            return Inventory->GetEquipmentCount(EIGIEquipmentType::ThermalViewer) > 0;

        case EIGIVisionMode::Normal:
        default:
            return true;
    }
}

float UIGIVisionComponent::GetFieldOfViewOverride() const
{
    return IsBinocularsActive() ? CurrentBinocularFov : 0.0f;
}

void UIGIVisionComponent::RefreshPostProcess()
{
    if (!IsValid(VisionPostProcess))
    {
        return;
    }

    // This component owns its own post-process settings, so rebuilding them on
    // every mode switch cannot disturb the map's normal Post Process Volume.
    VisionPostProcess->Settings = FPostProcessSettings();

    if (ActiveMode == EIGIVisionMode::Normal)
    {
        return;
    }

    // Useful source-only fallback grading. Production materials can layer noise,
    // lens masks, phosphor bloom, and proper CustomStencil thermal rendering.
    switch (ActiveMode)
    {
        case EIGIVisionMode::Binoculars:
            VisionPostProcess->Settings.bOverride_VignetteIntensity = true;
            VisionPostProcess->Settings.VignetteIntensity = 0.55f;
            break;

        case EIGIVisionMode::NightVision:
            VisionPostProcess->Settings.bOverride_SceneColorTint = true;
            VisionPostProcess->Settings.SceneColorTint = FLinearColor(0.18f, 1.0f, 0.20f, 1.0f);
            VisionPostProcess->Settings.bOverride_ColorSaturation = true;
            VisionPostProcess->Settings.ColorSaturation = FVector4(0.20f, 1.15f, 0.20f, 1.0f);
            VisionPostProcess->Settings.bOverride_AutoExposureBias = true;
            VisionPostProcess->Settings.AutoExposureBias = 1.25f;
            VisionPostProcess->Settings.bOverride_BloomIntensity = true;
            VisionPostProcess->Settings.BloomIntensity = 0.85f;
            VisionPostProcess->Settings.bOverride_VignetteIntensity = true;
            VisionPostProcess->Settings.VignetteIntensity = 0.40f;
            break;

        case EIGIVisionMode::Thermal:
            VisionPostProcess->Settings.bOverride_SceneColorTint = true;
            VisionPostProcess->Settings.SceneColorTint = FLinearColor(1.0f, 0.48f, 0.12f, 1.0f);
            VisionPostProcess->Settings.bOverride_ColorSaturation = true;
            VisionPostProcess->Settings.ColorSaturation = FVector4(0.45f, 0.45f, 0.45f, 1.0f);
            VisionPostProcess->Settings.bOverride_ColorContrast = true;
            VisionPostProcess->Settings.ColorContrast = FVector4(1.30f, 1.30f, 1.30f, 1.0f);
            VisionPostProcess->Settings.bOverride_VignetteIntensity = true;
            VisionPostProcess->Settings.VignetteIntensity = 0.25f;
            break;

        case EIGIVisionMode::Normal:
        default:
            break;
    }

    if (UMaterialInterface* Material = ResolveMaterialForMode(ActiveMode);
        IsValid(Material))
    {
        VisionPostProcess->Settings.AddBlendable(
            Material,
            GetPostProcessWeightForMode(ActiveMode));
    }
}

UMaterialInterface* UIGIVisionComponent::ResolveMaterialForMode(
    const EIGIVisionMode Mode) const
{
    switch (Mode)
    {
        case EIGIVisionMode::Binoculars:
            return BinocularPostProcessMaterial.IsNull()
                ? nullptr
                : BinocularPostProcessMaterial.LoadSynchronous();

        case EIGIVisionMode::NightVision:
            return NightVisionPostProcessMaterial.IsNull()
                ? nullptr
                : NightVisionPostProcessMaterial.LoadSynchronous();

        case EIGIVisionMode::Thermal:
            return ThermalPostProcessMaterial.IsNull()
                ? nullptr
                : ThermalPostProcessMaterial.LoadSynchronous();

        case EIGIVisionMode::Normal:
        default:
            return nullptr;
    }
}

float UIGIVisionComponent::GetPostProcessWeightForMode(
    const EIGIVisionMode Mode) const
{
    switch (Mode)
    {
        case EIGIVisionMode::Binoculars:
            return BinocularPostProcessWeight;

        case EIGIVisionMode::NightVision:
            return NightVisionPostProcessWeight;

        case EIGIVisionMode::Thermal:
            return ThermalPostProcessWeight;

        case EIGIVisionMode::Normal:
        default:
            return 0.0f;
    }
}
