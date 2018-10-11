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

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	
};
