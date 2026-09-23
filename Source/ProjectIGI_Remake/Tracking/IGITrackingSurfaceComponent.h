#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tracking/BDFRTrackingTypes.h"
#include "IGITrackingSurfaceComponent.generated.h"

class UBDFRTrackEmitterComponent;

UCLASS(ClassGroup = (IGI), meta = (BlueprintSpawnableComponent))
class PROJECTIGI_REMAKE_API UIGITrackingSurfaceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UIGITrackingSurfaceComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "IGI|Tracking")
	EBDFRTrackSurfaceType GetCurrentTrackSurface() const { return CurrentSurface; }

	UFUNCTION(BlueprintCallable, Category = "IGI|Tracking")
	void RefreshTrackingSurface();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Tracking", meta = (ClampMin = "50.0"))
	float TraceDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Tracking", meta = (ClampMin = "0.05"))
	float UpdateIntervalSeconds = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IGI|Tracking")
	TEnumAsByte<ECollisionChannel> GroundTraceChannel = ECC_Visibility;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBDFRTrackEmitterComponent> TrackEmitter;

	UPROPERTY(Transient)
	EBDFRTrackSurfaceType CurrentSurface = EBDFRTrackSurfaceType::Default;

	static EBDFRTrackSurfaceType ConvertPhysicalSurface(EPhysicalSurface SurfaceType);
};
