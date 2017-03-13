// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Pathfinder.h"
#include "PathfinderGameMode.h"
#include "PathfinderPlayerController.h"
#include "PathfinderCharacter.h"

APathfinderGameMode::APathfinderGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APathfinderPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}