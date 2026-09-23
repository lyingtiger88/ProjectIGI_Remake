#include "Vision/IGIThermalSignatureComponent.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

UIGIThermalSignatureComponent::UIGIThermalSignatureComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UIGIThermalSignatureComponent::BeginPlay()
{
    Super::BeginPlay();
    SetThermalSignatureEnabled(bEnableOnBeginPlay);
}

void UIGIThermalSignatureComponent::SetThermalSignatureEnabled(const bool bEnabled)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    TArray<UPrimitiveComponent*> PrimitiveComponents;
    Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    const uint8 StencilValue = static_cast<uint8>(
        FMath::Clamp(CustomDepthStencilValue, 1, 255));

    for (UPrimitiveComponent* Primitive : PrimitiveComponents)
    {
        if (!IsValid(Primitive))
        {
            continue;
        }

        Primitive->SetRenderCustomDepth(bEnabled);

        if (bEnabled)
        {
            Primitive->SetCustomDepthStencilValue(StencilValue);
        }
    }

    bThermalSignatureEnabled = bEnabled;
}
