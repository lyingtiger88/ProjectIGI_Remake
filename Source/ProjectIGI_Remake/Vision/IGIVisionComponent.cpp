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

    VisionPostProcess->Settings.WeightedBlendables.Array.Reset();

    if (ActiveMode == EIGIVisionMode::Normal)
    {
        return;
    }

    UMaterialInterface* Material = ResolveMaterialForMode(ActiveMode);
    if (!IsValid(Material))
    {
        return;
    }

    VisionPostProcess->Settings.AddBlendable(
        Material,
        GetPostProcessWeightForMode(ActiveMode));
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
