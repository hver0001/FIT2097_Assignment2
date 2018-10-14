// Fill out your copyright notice in the Description page of Project Settings.

#include "BinaryLock.h"
#include "A2GameMode.h"
#include "Net/UnrealNetwork.h"

//Constructor
ABinaryLock::ABinaryLock() {
	//Set the default binary state that's correct
	//ENSURE THAT THE NUMBER OF STATES HERE IS AT LEAST THE NUMBER OF CONNECTED SWITCHES
	CorrectBinaryStates = { false, false, false, false, false, false };

	bIsComplete = false;
}

//Networking code
void ABinaryLock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABinaryLock, bIsComplete);
}

//Check if all switches map to the correct binary
void ABinaryLock::CheckBinary() {
	if (Role == ROLE_Authority) {
		bool correct = true;

		//Loop through all connected switches
		for (int i = 0; i < BinarySwitches.Num(); i++) {
			if (ASwitch* s = BinarySwitches[i]) {
				//If any switch does not match, then fail the binary
				if (s->IsActive() == CorrectBinaryStates[i]) {
					correct = false;
				}
				else {
				}
			}
		}

		//Check to see if the binary state has changed
		if (bIsComplete != correct) {
			bIsComplete = correct;

			//Call server multicast function
			ClientOnComplete();
		}
	}
}

//Returns if the binary is complete or not
bool ABinaryLock::IsComplete()
{
	return bIsComplete;
}

//This is called whenever the binary state is updated
void ABinaryLock::OnRep_IsComplete()
{

}

//Functionality when the binary is updated
void ABinaryLock::WasCompleted_Implementation()
{
	//Log a debug message
	//The %s adds the string from the GetName function to the output log
}

//Add the _Implementation for the code behind the client picked up function - MULTICAST event
void ABinaryLock::ClientOnComplete_Implementation()
{
	//Fire the Blueprint Native Event, which itself cannot be replicated
	WasCompleted();

	//Ensure the door has been connected
	if (ConnectedDoor != NULL) {
		//Lock or unlock the door depending if the binary has been completed
		ConnectedDoor->SetLocked(!bIsComplete);

		//If the door is complete
		if (bIsComplete) {
			//Will be called twice, but only needs to call on server
			if (Role == ROLE_Authority) {
				//Update information text in gamemode
				if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
					gameMode->UpdateInformationText("The binary code is correct - A new door is unlocked!");
				}
			}
		}
	}
}
