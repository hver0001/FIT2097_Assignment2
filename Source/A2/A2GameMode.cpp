// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "A2GameMode.h"
#include "A2HUD.h"
#include "A2Character.h"
#include "A2GameState.h"
#include "Door.h"
#include "FuseLock.h"
#include "BinaryLock.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

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
	HealthRate = 0.2f;
	HealthDrainDelay = 0.1f;
	InformationTextRemoveDelay = 5.0f;

	//Starts the game as unpaused
	CurrentGameState = EGameState::Start;
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

	//Pause the game initially
	SetGameState(EGameState::Start);
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

//Unlocks a particular door
void AA2GameMode::UnlockDoor(int keyId) {
	//Get world
	UWorld* World = GetWorld();
	check(World);

	//Loop through all game objects
	for (TActorIterator<ADoor> Door(World); Door; ++Door) {
		//Check if key is identical
		if (Door->GetKeyRequired() == keyId) {
			UE_LOG(LogClass, Log, TEXT("Door Unlocked with key: %i"), keyId);
			//Unlock the door
			Door->SetLocked(false);

			//Output an update on the state
			UpdateInformationText("A key has been found! A new door has been unlocked!");
		}
	}

	//Get the game state
	if (AA2GameState* MyGameState = Cast<AA2GameState>(GameState)) {
		//Set the unlocked set of variables to true for HUD
		MyGameState->SetKeyValue(keyId - 1, true);
	}
}

//Updates all fuse locks
void AA2GameMode::UpdateFuse() {
	//Get world
	UWorld* World = GetWorld();
	check(World);

	//Loop through all game objects that are fuse locks
	for (TActorIterator<AFuseLock> Fuse(World); Fuse; ++Fuse) {		
		//Update the fuse
		Fuse->SetFuse();
	}
}

//Updates all binary objects
void AA2GameMode::CheckBinary() {
	//Get world
	UWorld* World = GetWorld();
	check(World);

	//Loop through all game objects that are binary locks
	for (TActorIterator<ABinaryLock> Binary(World); Binary; ++Binary) {
		//Check the binary
		Binary->CheckBinary();
	}
}

//Updates the information text on the game state
void AA2GameMode::UpdateInformationText(FString NewInfo) {
	//Checks to see if game state exists
	if (AA2GameState* MyGameState = Cast<AA2GameState>(GameState)) {
		MyGameState->SetInformation(NewInfo);
	}

	//Cancel previous timer
	GetWorldTimerManager().ClearTimer(InformationTextRemoveTimer);

	//Create a timer to remove text after a specified amount of time
	GetWorldTimerManager().SetTimer(InformationTextRemoveTimer, this, &AA2GameMode::ClearInformationText, InformationTextRemoveDelay, false);	
}

//Change the state of the game
void AA2GameMode::SetGameState(EGameState NewGameState)
{
	//Change the game state
	CurrentGameState = NewGameState;

	//Get world
	UWorld* World = GetWorld();
	check(World);

	//Go through all the characters in the game using a unreal function
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It) {
		//Attempt to check for a player controller using a cast
		if (APlayerController* PlayerController = Cast<APlayerController>(*It)) {
			//Attempt to check for a controller using a cast
			if (AA2Character* Character = Cast<AA2Character>(PlayerController->GetPawn()))
			{
				Character->UpdateGameState(CurrentGameState);
			}
		}
	}
	
	//Pause the game world
	UGameplayStatics::SetGamePaused(World, CurrentGameState != EGameState::Playing);
}

//Clear the information text in the game state
void AA2GameMode::ClearInformationText() {
	if (AA2GameState* MyGameState = Cast<AA2GameState>(GameState)) {
		MyGameState->ClearInformationText();
	}
}
