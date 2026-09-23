#include "AI/IGIDogAIController.h"

#include "IGIPlayerCharacter.h"
#include "Vision/IGIThermalSignatureComponent.h"

void AIGIDogAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!IsValid(InPawn) ||
		IsValid(InPawn->FindComponentByClass<UIGIThermalSignatureComponent>()))
	{
		return;
	}

	UIGIThermalSignatureComponent* ThermalSignature =
		NewObject<UIGIThermalSignatureComponent>(InPawn, TEXT("IGIThermalSignature"));

	if (IsValid(ThermalSignature))
	{
		InPawn->AddInstanceComponent(ThermalSignature);
		ThermalSignature->RegisterComponent();
	}
}

bool AIGIDogAIController::BDFR_ShouldProcessPerceivedActor_Implementation(AActor* SourceActor) const
{
	return IsValid(SourceActor) && SourceActor->IsA<AIGIPlayerCharacter>();
}
