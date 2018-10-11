// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"


// Sets default values
AInteractable::AInteractable()
{
	//Tell the engine to replicate this actor
	bReplicates = true;

	//Lamps do not need to tick every frame (waste of CPU cycles for this actor)
	PrimaryActorTick.bCanEverTick = false;

	//StaticMeshActor disables overlap events by default, which we need to re-enable
	//GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	//Starts the interactable to be active if needed
	SetActive(bStartActive);

}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
}

//Required for networking functions
void AInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); //Run the super script for networking data

	//For the entire instance of this class, the class and variable should be replicated
	//NOTE: This macro requires the networking code to be included
	DOREPLIFETIME(AInteractable, bIsActive);
	DOREPLIFETIME(AInteractable, bStartActive);
}

//Returns whether the object is active or not
bool AInteractable::IsActive()
{
	return bIsActive;
}

//Mutator of the on variable for changing states
void AInteractable::SetActive(bool NewState)
{
	//Only the server can change the state of the interactable object state
	if (Role == ROLE_Authority) {
		bIsActive = NewState;
		ClientOnToggle();
	}
}

//Returns the name of the interactable item
FString AInteractable::GetInteractableName()
{
	return InteractableName;
}

//This is called whenever the active boolean is updated
void AInteractable::OnRep_IsActive()
{

}

//Functionality when the interactable object is toggled
void AInteractable::WasToggled_Implementation()
{
	//Log a debug message
	//The %s adds the string from the GetName function to the output log
}

//Add the _Implementation for the code behind the client picked up function - MULTICAST event
void AInteractable::ClientOnToggle_Implementation()
{
	//Fire the Blueprint Native Event, which itself cannot be replicated
	WasToggled();
}


