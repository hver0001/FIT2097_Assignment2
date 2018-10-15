// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "GameFramework/GameModeBase.h"
#include "A2GameMode.generated.h"

UCLASS(minimalapi)
class AA2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AA2GameMode();

	//Start the life level decrease timer at beginning of game
	virtual void BeginPlay() override;

	//Get the starting health
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetStartingHealth();

	//Get the current rate of health decay
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealthRate();

	//Unlocks a particular door
	UFUNCTION(BlueprintCallable, Category = "Key")
		void UnlockDoor(int keyId);

	//Updates the fuse lock when fuses are inserted
	UFUNCTION(BlueprintCallable, Category = "Fuse")
		void UpdateFuse();

	//Updates all binary code that exist when a switch is pressed
	UFUNCTION(BlueprintCallable, Category = "Binary")
		void CheckBinary();

	//Updates the game state information text
	UFUNCTION(BlueprintCallable, Category = "Information")
		void UpdateInformationText(FString NewInfo);

	//Pauses/Resumes the game (stops character and variables from changing)
	UFUNCTION(BlueprintCallable, Category = "State")
		void TogglePause();


protected:
	//Stores the rate of the delay that the characters will experience (how often to update)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		float HealthDrainDelay;

	//Access to the timer for draining the health
	FTimerHandle HealthDrainTimer;

	//Stores the amount of health a player loses each second
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", Meta = (BlueprintProtected = "true"))
		float StartingHealth;

	//Stores the amount of health a player loses each second
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", Meta = (BlueprintProtected = "true"))
		float HealthRate;

	//Access the timer for removing the text from the screen after a period of time
	FTimerHandle InformationTextRemoveTimer;

	//The time it takes for text to be removed
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Information", Meta = (BlueprintProtected = "true"))
		float InformationTextRemoveDelay;

	//Stores whether the game is paused or not
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State", Meta = (BlueprintProtected = "true"))
		bool bIsPaused;

private:
	//Drains health from characters and updates gameplay
	void DrainHealthOverTime();

	//Clears the information text from the game state
	UFUNCTION()
		void ClearInformationText();
};



