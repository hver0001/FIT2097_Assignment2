// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/GameStateBase.h"
#include "A2GameState.generated.h"


//Creates an enumerator for the different game states
UENUM(BlueprintType) //BlueprintType
enum class EGameState : uint8 {
	Start		UMETA(DisplayName = "Start Menu"),
	Playing		UMETA(DisplayName = "Game Menu"),
	Paused		UMETA(DisplayName = "Pause Menu"),
	Dead		UMETA(DisplayName = "Loss Menu"),
	Won			UMETA(DisplayName = "Win Menu")
};

/**
 * 
 */
UCLASS()
class A2_API AA2GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AA2GameState();

	//Max health of players (and starting health)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
		float MaxHealth;

	//Returns the max health
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetMaxHealth();

	//State of the keys
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Keys")
		TArray<bool> KeySet;

	//Returns the state of the keys
	UFUNCTION(BlueprintCallable, Category = "Keys")
		TArray<bool> GetKeySet();

	//Changes the state of the keys
	UFUNCTION(BlueprintCallable, Category = "Keys")
		void SetKeyValue(int keyIndex, bool bValue);

	//Stores the information text - e.g "A door has been opened"
	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = "Information")
		FString InformationText;

	//Sets the information text
	UFUNCTION(BlueprintCallable, Category = "Information")
		FString GetInformationText();

	//Updates the information text
	UFUNCTION(BlueprintCallable, Category = "Information")
		void SetInformation(FString NewInfo);

	//Clears the information text after the timer runs out
	UFUNCTION(BlueprintCallable, Category = "Information")
		void ClearInformationText();

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Stores the current game state of the game (playing, death, ect)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
		EGameState CurrentGameState;

	//Sets a new game state
	UFUNCTION(BlueprintCallable, Category = "State")
		void SetGameState(EGameState NewGameState);

	//Gets the game satte
	UFUNCTION(BlueprintCallable, Category = "State")
		EGameState GetGameState();
};
