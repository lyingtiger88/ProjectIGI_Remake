#include "Game/IGIGameModeBase.h"

#include "Game/IGIPlayerController.h"
#include "IGIPlayerCharacter.h"
#include "UI/IGIHUD.h"

AIGIGameModeBase::AIGIGameModeBase()
{
	DefaultPawnClass = AIGIPlayerCharacter::StaticClass();
	PlayerControllerClass = AIGIPlayerController::StaticClass();
	HUDClass = AIGIHUD::StaticClass();
}
