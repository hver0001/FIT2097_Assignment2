// Fill out your copyright notice in the Description page of Project Settings.

#include "Switch.h"

//Constructor
ASwitch::ASwitch() {

}

//Same function
void ASwitch::SetActive(bool NewState)
{
	//Ensure the door has been connected
	if (ConnectedDoor != NULL) {
		//Unlock the connected door
		ConnectedDoor->SetLocked(NewState);
	}

	Super::SetActive(NewState);
}




