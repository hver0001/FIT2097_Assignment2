// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "A2GameMode.h"
#include "A2HUD.h"
#include "A2Character.h"
#include "A2GameState.h"
#include "UObject/ConstructorHelpers.h"

AA2GameMode::AA2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	//Use our custom HUD class
	//HUDClass = AA2HUD::StaticClass();
	static ConstructorHelpers::FClassFinder<AA2HUD>PlayerHUDClass(TEXT("/Game/Blueprints/BP_HUD"));
	if (PlayerHUDClass.Class != NULL) {
		HUDClass = PlayerHUDClass.Class;
	}

	//Set the GameState used in the game
	GameStateClass = AA2GameState::StaticClass();
	if (!GameStateClass) {
		UE_LOG(LogTemp, Error, TEXT("FAILED: GameState failed to set in %s!"), *this->GetName());
	}

	//Base health values
	StartingHealth = 100.f;
	HealthRate = 0.1f;
	HealthDrainDelay = 0.1f;
}

void AA2GameMode::BeginPlay() {
	//Set the recurring timer
	GetWorldTimerManager().SetTimer(HealthDrainTimer, this, &AA2GameMode::DrainHealthOverTime, HealthDrainDelay, true);

	//Access the world to get the players
	UWorld* World = GetWorld();
	check(World);

	//Go through all the characters in the game using a unreal function
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
		//Attempt to check for a player controller using a cast
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			//Attempt to check for a controller using a cast
			if (AA2Character* Character = Cast<AA2Character>(PlayerController->GetPawn()))
			{
				//Set the game state max health to the starting health
				if (AA2GameState* MyGameState = Cast<AA2GameState>(GameState)) {
					MyGameState->MaxHealth = Character->GetMaxHealth();
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("FAILED: GameState failed to be cast in %s!"), *this->GetName());
				}

				break;
			}
		}
	}
}

//Return the initial health of the players
float AA2GameMode::GetStartingHealth() {
	return StartingHealth;
}

//Return the amount of health the player loses per second
float AA2GameMode::GetHealthRate() {
	return HealthRate;
}

//Every time the timer loops, this function is called
void AA2GameMode::DrainHealthOverTime()
{
	//There is no need for an authority case as the gamemode only ever exists on the server

	//A check macro will check if the world exists and if not, crash the game, and this saves on performance time
	//After shipping the game, this code will get removed from the game, as it will be assumed the world exists so it will not need to check
	UWorld* World = GetWorld();
	check(World);

	//Go through all the characters in the game using a unreal function
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
		//Attempt to check for a player controller using a cast
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			//Attempt to check for a specific controller using a cast
			if (AA2Character* Character = Cast<AA2Character>(PlayerController->GetPawn()))
			{
				//Check if health exists, if so, update the health
				if (Character->GetCurrentHealth() > 0) {
					Character->UpdateHealth(-HealthRate);
				}
				else { //No health left of the character

				}
			}
		}
	}
}
