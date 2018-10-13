// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class A2_API ADoor : public AInteractable
{
	GENERATED_BODY()
	
public:
	//Constructor
	ADoor();

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Gets the required key
	UFUNCTION(BlueprintPure, Category = "Door")
		int GetKeyRequired();

	//This function returns whether or not the door is locked
	UFUNCTION(BlueprintPure, Category = "Door")
		bool IsLocked();

	//This allows other classes to safely change the doors locked state
	UFUNCTION(BlueprintCallable, Category = "Door")
		virtual void SetLocked(bool NewState);

	//Function to call when door is unlocked
	//A blueprint native event allows to put some functionality in blueprint and in C++
	UFUNCTION(BlueprintNativeEvent, Category = "Door")
		void WasUnlocked();
	virtual void WasUnlocked_Implementation();

	//Same as interactable function, but checks to see if door is unlocked
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		void SetActive(bool NewState) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//The key required to unlock the door - a ZERO means the door requires no keys
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door", Meta = (BlueprintProtected = "true"))
		int keyRequired;

	//If the door is locked or unlocked
	UPROPERTY(ReplicatedUsing = OnRep_IsLocked, EditAnywhere, BlueprintReadWrite, Category = "Door", Meta = (BlueprintProtected = "true"))
		bool bIsLocked;

	//Called whenever IsLocked is updated
	UFUNCTION()
		virtual void OnRep_IsLocked();

private:
	//Client side handling of being locked or unlocked
	//The multicast calls out to all servers and clients
	//Only this class needs to call this function
	UFUNCTION(NetMulticast, Unreliable)
		void ClientOnUnlock();
	
};
