// Fill out your copyright notice in the Description page of Project Settings.

#include "A2GameState.h"
#include "Net/UnrealNetwork.h"

//Constructor
AA2GameState::AA2GameState() {

}

//Set networked variables
void AA2GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	//Run the super script for networking data
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Allow variables to be replicated
	DOREPLIFETIME(AA2GameState, MaxHealth);
}



