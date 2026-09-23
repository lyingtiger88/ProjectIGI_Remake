#include "Tracking/IGITrackingSurfaceComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Tracking/BDFRTrackEmitterComponent.h"

UIGITrackingSurfaceComponent::UIGITrackingSurfaceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UIGITrackingSurfaceComponent::BeginPlay()
{
	Super::BeginPlay();

	TrackEmitter = GetOwner() != nullptr
		? GetOwner()->FindComponentByClass<UBDFRTrackEmitterComponent>()
		: nullptr;

	PrimaryComponentTick.TickInterval = UpdateIntervalSeconds;
	RefreshTrackingSurface();
}

void UIGITrackingSurfaceComponent::TickComponent(
	const float DeltaTime,
	const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	RefreshTrackingSurface();
}

void UIGITrackingSurfaceComponent::RefreshTrackingSurface()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(Owner) || !IsValid(World))
	{
		return;
	}

	if (!IsValid(TrackEmitter))
	{
		TrackEmitter = Owner->FindComponentByClass<UBDFRTrackEmitterComponent>();
	}

	if (!IsValid(TrackEmitter))
	{
		return;
	}

	const FVector Start = Owner->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	const FVector End = Owner->GetActorLocation() - FVector(0.0f, 0.0f, TraceDistance);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(IGITrackingSurface), false, Owner);
	Params.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (!World->LineTraceSingleByChannel(Hit, Start, End, GroundTraceChannel, Params))
	{
		CurrentSurface = EBDFRTrackSurfaceType::Default;
		TrackEmitter->SetSurfaceType(CurrentSurface);
		return;
	}

	const UPhysicalMaterial* PhysicalMaterial = Hit.PhysMaterial.Get();
	const EPhysicalSurface PhysicalSurface = IsValid(PhysicalMaterial)
		? UPhysicalMaterial::DetermineSurfaceType(PhysicalMaterial)
		: SurfaceType_Default;

	CurrentSurface = ConvertPhysicalSurface(PhysicalSurface);
	TrackEmitter->SetSurfaceType(CurrentSurface);
}

EBDFRTrackSurfaceType UIGITrackingSurfaceComponent::ConvertPhysicalSurface(const EPhysicalSurface SurfaceType)
{
	// Project convention from DefaultEngine.ini:
	// 1 Dirt, 2 Mud, 3 Snow, 4 Sand, 5 Grass, 6 Concrete, 7 Metal, 8 Water.
	switch (static_cast<uint8>(SurfaceType))
	{
		case 1: return EBDFRTrackSurfaceType::Dirt;
		case 2: return EBDFRTrackSurfaceType::Mud;
		case 3: return EBDFRTrackSurfaceType::Snow;
		case 4: return EBDFRTrackSurfaceType::Sand;
		case 5: return EBDFRTrackSurfaceType::Grass;
		case 6: return EBDFRTrackSurfaceType::Concrete;
		case 7: return EBDFRTrackSurfaceType::Metal;
		case 8: return EBDFRTrackSurfaceType::Water;
		default: return EBDFRTrackSurfaceType::Default;
	}
}
