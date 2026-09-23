#include "Game/IGIGameModeBase.h"

#include "IGIPlayerCharacter.h"

AIGIGameModeBase::AIGIGameModeBase()
{
	DefaultPawnClass = AIGIPlayerCharacter::StaticClass();
}
