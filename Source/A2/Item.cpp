// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "Net/UnrealNetwork.h"

//Constructor for setting basic components
AItem::AItem() {
	//By default set the item to a health pack
	ItemType = EItemTypes::Health;

	//Set default health values
	ItemValue = 25.f;
}

// Add networking replication code
void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Make the battery power replicated
	DOREPLIFETIME(AItem, ItemValue);
}

//Return the item type
EItemTypes AItem::GetItemType() {
	return ItemType;
}

//Return the item value
float AItem::GetItemValue() {
	return ItemValue;
}

//Called when the activation is set
void AItem::SetActive(bool NewState) {
	//Runs the super code for interactables
	Super::SetActive(NewState);

	if (Role == ROLE_Authority)
	{
		//Sets the lifespan of the item before the object is destroyed (in seconds)
		//This gives clients time to play VFX and sounds, etc, before destroying item
		SetLifeSpan(1.0f);
	}
}


