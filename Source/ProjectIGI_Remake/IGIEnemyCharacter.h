#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "Interfaces/BDFRLocomotionInterface.h"
#include "IGIEnemyCharacter.generated.h"

class UAlsAnimationInstance;
struct FGameplayTag;

UCLASS()
class PROJECTIGI_REMAKE_API AIGIEnemyCharacter : public AAlsCharacter, public IBDFRLocomotionInterface
{
	GENERATED_BODY()

public:
	AIGIEnemyCharacter();

	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BDFR_SetDesiredGait_Implementation(FGameplayTag GaitTag) override;
	virtual void BDFR_SetDesiredStance_Implementation(FGameplayTag StanceTag) override;
	virtual void BDFR_SetAiming_Implementation(bool bAiming) override;
	virtual void BDFR_SetLookTarget_Implementation(AActor* TargetActor) override;
	virtual void BDFR_ClearLookTarget_Implementation() override;

private:
	void RefreshAlsAnimationInstance();
};
