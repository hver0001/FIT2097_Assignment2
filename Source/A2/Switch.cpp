// Fill out your copyright notice in the Description page of Project Settings.

#include "Switch.h"
#include "A2GameMode.h"

//Constructor
ASwitch::ASwitch() {

}

//Same function
void ASwitch::SetActive(bool NewState)
{
	Super::SetActive(NewState);

	//Ensure the door has been connected
	if (ConnectedDoor != NULL) {
		//Unlock the connected door
		ConnectedDoor->SetLocked(NewState);

		//If unlocked
		if (!NewState) {
			//Only needs to be called on the server
			if (Role == ROLE_Authority) {
				//Update information text in gamemode
				if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
					gameMode->UpdateInformationText("A new door is unlocked!");
				}
			}
		}
	}
	//If not connected to a door, it could be a binary switch
	else {
		//Tell the gamemode to update the binary objects
		if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
			gameMode->CheckBinary();
		}
	}
}




