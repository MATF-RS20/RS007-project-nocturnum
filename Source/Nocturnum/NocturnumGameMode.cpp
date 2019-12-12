// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "NocturnumGameMode.h"
#include "NocturnumPlayerController.h"
#include "NocturnumCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANocturnumGameMode::ANocturnumGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ANocturnumPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}