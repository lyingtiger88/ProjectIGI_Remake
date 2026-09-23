#include "Game/IGIGameModeBase.h"

#include "Game/IGIPlayerController.h"
#include "IGIPlayerCharacter.h"

AIGIGameModeBase::AIGIGameModeBase()
{
	DefaultPawnClass = AIGIPlayerCharacter::StaticClass();
	PlayerControllerClass = AIGIPlayerController::StaticClass();
}
