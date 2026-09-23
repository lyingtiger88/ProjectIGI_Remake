#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IGIPlayerController.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "IGI Player Controller"))
class PROJECTIGI_REMAKE_API AIGIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AIGIPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void ApplyGameInputMode();
};
