#include "Game/IGIPlayerController.h"

AIGIPlayerController::AIGIPlayerController()
{
	bShowMouseCursor = false;
}

void AIGIPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ApplyGameInputMode();
}

void AIGIPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ApplyGameInputMode();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("IGI PlayerController possessed pawn: %s"),
		IsValid(InPawn) ? *InPawn->GetName() : TEXT("<none>"));
}

void AIGIPlayerController::ApplyGameInputMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	if (IsLocalController())
	{
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
	}
}
