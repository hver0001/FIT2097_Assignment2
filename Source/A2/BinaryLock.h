// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "Door.h"
#include "Switch.h"
#include "BinaryLock.generated.h"

/**
 * 
 */
UCLASS()
class A2_API ABinaryLock : public AInteractable
{
	GENERATED_BODY()
	
public:
	//Constructor
	ABinaryLock();

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Checks if all switches match to the correct binary state and if so, set the complete to true
	UFUNCTION(BlueprintCallable, Category = "Binary")
		void CheckBinary();

	//Returns whether or not the binary is complete
	UFUNCTION(BlueprintPure, Category = "Binary")
		bool IsComplete();

	//Function to call when 'binary state is changed
	UFUNCTION(BlueprintNativeEvent, Category = "Binary")
		void WasCompleted();
	virtual void WasCompleted_Implementation();
	

protected:
	//True when the binary is complete
	UPROPERTY(ReplicatedUsing = OnRep_IsComplete, EditAnywhere, BlueprintReadWrite, Category = "Binary", Meta = (BlueprintProtected = "true"))
		bool bIsComplete;

	//Called whenever IsActive is updated
	UFUNCTION()
		virtual void OnRep_IsComplete();

	//Holds the connection to all the switches that contribute to the binary state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binary", Meta = (BlueprintProtected = "true"))
		TArray<ASwitch*> BinarySwitches;

	//Holds the correct state of the binary
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Binary", Meta = (BlueprintProtected = "true"))
		TArray<bool> CorrectBinaryStates;

	//Stores the connected door for when the fuse is completed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
		ADoor* ConnectedDoor;

private:
	//The multicast calls out to all servers and clients
	UFUNCTION(NetMulticast, Unreliable)
		void ClientOnComplete();
};
