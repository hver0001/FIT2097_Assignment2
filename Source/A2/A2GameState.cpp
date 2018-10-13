// Fill out your copyright notice in the Description page of Project Settings.

#include "A2GameState.h"
#include "Net/UnrealNetwork.h"

//Constructor
AA2GameState::AA2GameState() {
	//Initialise the key set
	KeySet = { false, false, false };
}

//Set networked variables
void AA2GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Run the super script for networking data
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Allow variables to be replicated
	DOREPLIFETIME(AA2GameState, MaxHealth);
	DOREPLIFETIME(AA2GameState, KeySet)
}

//Return the max health
float AA2GameState::GetMaxHealth() {
	return MaxHealth;
}

//Return the state of the keys collected
TArray<bool> AA2GameState::GetKeySet() {
	return KeySet;
}

//Sets the value of a key
void AA2GameState::SetKeyValue(int keyIndex, bool bValue)
{
	KeySet[keyIndex] = bValue;
}

