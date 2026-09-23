#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "Interfaces/BDFRAssistanceInterface.h"
#include "Interfaces/BDFRLocomotionInterface.h"
#include "IGIEnemyCharacter.generated.h"

class UAlsAnimationInstance;
class UBDFRAcousticExposureComponent;
class UBDFRDistressComponent;
class UBDFRHealthComponent;
class UBDFRInjuryResponseComponent;
struct FGameplayTag;

UCLASS()
class PROJECTIGI_REMAKE_API AIGIEnemyCharacter
	: public AAlsCharacter
	, public IBDFRLocomotionInterface
	, public IBDFRAssistanceInterface
{
	GENERATED_BODY()

public:
	AIGIEnemyCharacter();

	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Health")
	UBDFRHealthComponent* GetBDFRHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Health")
	UBDFRDistressComponent* GetBDFRDistressComponent() const { return DistressComponent; }

	UFUNCTION(BlueprintPure, Category = "IGI|AI|Acoustics")
	UBDFRAcousticExposureComponent* GetBDFRAcousticExposureComponent() const { return AcousticExposureComponent; }

protected:
	virtual void BDFR_SetDesiredGait_Implementation(FGameplayTag GaitTag) override;
	virtual void BDFR_SetDesiredStance_Implementation(FGameplayTag StanceTag) override;
	virtual void BDFR_SetAiming_Implementation(bool bAiming) override;
	virtual void BDFR_SetLookTarget_Implementation(AActor* TargetActor) override;
	virtual void BDFR_ClearLookTarget_Implementation() override;

	virtual bool BDFR_CanReceiveAssistance_Implementation(AActor* Helper) const override;
	virtual FVector BDFR_GetAssistanceLocation_Implementation(AActor* Helper) const override;
	virtual void BDFR_BeginAssistance_Implementation(AActor* Helper) override;
	virtual void BDFR_CompleteAssistance_Implementation(AActor* Helper) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|AI|Health")
	TObjectPtr<UBDFRHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|AI|Health")
	TObjectPtr<UBDFRDistressComponent> DistressComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|AI|Health")
	TObjectPtr<UBDFRInjuryResponseComponent> InjuryResponseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "IGI|AI|Acoustics")
	TObjectPtr<UBDFRAcousticExposureComponent> AcousticExposureComponent;

private:
	void RefreshAlsAnimationInstance();
};
