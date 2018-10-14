// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/GameStateBase.h"
#include "A2GameState.generated.h"

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

	//Sets the information text - e.g "A door has been opened"
	UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = "Information")
		FString InformationText;

	//Updates the information text
	UFUNCTION(BlueprintCallable, Category = "Information")
		void SetInformation(FString NewInfo);

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	
};
