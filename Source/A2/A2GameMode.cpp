// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "A2GameMode.h"
#include "A2HUD.h"
#include "A2Character.h"
#include "UObject/ConstructorHelpers.h"

AA2GameMode::AA2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AA2HUD::StaticClass();
}
