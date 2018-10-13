// Fill out your copyright notice in the Description page of Project Settings.

#include "FuseLock.h"
#include "Net/UnrealNetwork.h"

//Constructor
AFuseLock::AFuseLock() {

	//Sets the array of fuses (2 fuses in this case)
	RequiredFuses = { false, false };
}

void AFuseLock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFuseLock, RequiredFuses);
}

//Returns the current state of all fuses for the lock
TArray<bool> AFuseLock::GetRequiredFuses() {
	return RequiredFuses;
}

//Sets the next element of the fuse
void AFuseLock::SetFuse() {
	if (Role == ROLE_Authority) {
		UE_LOG(LogClass, Warning, TEXT("A fuse has been updated"));
		//Check to see if the first fuse is false and if so, make it true
		if (!RequiredFuses[0])
			RequiredFuses[0] = true;
		else if (!RequiredFuses[1])
			RequiredFuses[1] = true;
		else {
			//Invalid fuse count
		}

		//Call the MultiCast function
		ClientFuseChange();
	}
}

//Called whenever the fuse state changes
void AFuseLock::FuseUpdate_Implementation() {
	
}

//Multicast function for when the fuse state changes
void AFuseLock::ClientFuseChange_Implementation() {
	//Fire the Blueprint Native Event, which itself cannot be replicated
	FuseUpdate();
}


