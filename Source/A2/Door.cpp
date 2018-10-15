// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Net/UnrealNetwork.h"

//Constructor
ADoor::ADoor() {
	//By default, door is locked
	bIsLocked = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

//Required for networking functions
void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); //Run the super script for networking data

	//For the entire instance of this class, the class and variable should be replicated
	//NOTE: This macro requires the networking code to be included
	DOREPLIFETIME(ADoor, bIsLocked);
}

//Returns whether the door is locked or not
bool ADoor::IsLocked()
{
	return bIsLocked;
}

//Returns the required key
int ADoor::GetKeyRequired()
{
	return keyRequired;
}

//Mutator of the on variable for unlocking the door
void ADoor::SetLocked(bool NewState)
{
	//Only the server can change the state of the door lock
	if (Role == ROLE_Authority) {
		bIsLocked = NewState;
		ClientOnUnlock();
	}
}

//This is called whenever the locked boolean is updated
void ADoor::OnRep_IsLocked()
{

}

//Functionality when the door is unlocked
void ADoor::WasUnlocked_Implementation()
{

}

//Add the _Implementation for the code behind the client picked up function - MULTICAST event
void ADoor::ClientOnUnlock_Implementation()
{
	//Fire the Blueprint Native Event, which itself cannot be replicated
	WasUnlocked();
}

//Same function as interactable one, but check if door is locked or not to activate door
void ADoor::SetActive(bool NewState)
{
	//Only if the door is unlocked or attempting to close the door
	if (!bIsLocked || !NewState) {
		//Call the interactable function
		Super::SetActive(NewState);
	}
}




