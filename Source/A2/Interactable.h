// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class A2_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:
	//Constructor function
	AInteractable();

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Blueprint pure allows the blueprints to edit the function and call it
	//This function returns whether or not the interactable is on
	UFUNCTION(BlueprintPure, Category = "Interactable")
		bool IsActive();

	//This allows other classes to safely change the objects's activation state
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		virtual void SetActive(bool NewState);

	//Function to call when pickup is collected
	//A blueprint native event allows to put some functionality in blueprint and in C++
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable")
		void WasToggled();
	virtual void WasToggled_Implementation();

	//This allows other classes to find the name of the object type
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		FString GetInteractableName();

	//Gets the interactble available text
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		FString GetAvailableText();

	//Gets the interactble unavailable text
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		FString GetUnavailableText();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//True when the interactable is on, False when it is off
	UPROPERTY(ReplicatedUsing = OnRep_IsActive, EditAnywhere, BlueprintReadWrite, Category = "Interactable", Meta = (BlueprintProtected = "true"))
		bool bIsActive;

	//Called whenever IsActive is updated
	UFUNCTION()
		virtual void OnRep_IsActive();

	//Set the starting state of the interactable
	//The BlueprintProtected allows the blueprint to access this variable
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Interactable", Meta = (BlueprintProtected = "true"))
		bool bStartActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
		FString InteractableName;

	//Holds text for if the player can do something
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Iteractable")
		FString AvailableText;

	//Holds text for if the player cannot do something
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Iteractable")
		FString UnavailableText;

private:
	//Client side handling of being picked up
	//The multicast calls out to all servers and clients
	//Only this class needs to call this function
	UFUNCTION(NetMulticast, Unreliable)
		void ClientOnToggle();
	
};
