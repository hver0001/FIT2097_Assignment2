// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "FuseLock.generated.h"

/**
 * 
 */
UCLASS()
class A2_API AFuseLock : public AInteractable
{
	GENERATED_BODY()
	
public:
	//Constructor
	AFuseLock();

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Gets the set of fuse states
	UFUNCTION(BlueprintCallable, Category = "Fuse")
		TArray<bool> GetRequiredFuses();

	//Sets the next fuse state
	UFUNCTION(BlueprintCallable, Category = "Fuse")
		void SetFuse();

	//Function to call when the state of the fuse is updated
	UFUNCTION(BlueprintNativeEvent, Category = "Door")
		void FuseUpdate();
	virtual void FuseUpdate_Implementation();
	
protected:
	//Stores all possible requirements for the fuse needed
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Fuse", Meta = (BlueprintProtected = "true"))
		TArray<bool> RequiredFuses;

private:
	//Client side handling of fuse state changing
	UFUNCTION(NetMulticast, Unreliable)
		void ClientFuseChange();
	
};
