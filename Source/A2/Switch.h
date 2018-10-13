// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Door.h"
#include "Switch.generated.h"

/**
 * 
 */
UCLASS()
class A2_API ASwitch : public AInteractable
{
	GENERATED_BODY()
	
public:
	//Constructor
	ASwitch();

	//Same as interactable function, but opens a door connected
	UFUNCTION(BlueprintCallable, Category = "Door")
		void SetActive(bool NewState) override;

protected:
	//This is the door connected to the switch
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		ADoor* ConnectedDoor;
	
	
};
