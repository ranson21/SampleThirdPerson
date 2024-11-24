// Copyright Epic Games, Inc. All Rights Reserved.

#include "SampleThirdPersonGameMode.h"
#include "SampleThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASampleThirdPersonGameMode::ASampleThirdPersonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
