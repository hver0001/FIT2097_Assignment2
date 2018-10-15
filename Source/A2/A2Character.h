// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "GameFramework/Character.h"
#include "Interactable.h"
#include "Item.h"
#include "A2GameState.h"
#include "A2Character.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AA2Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AA2Character();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AA2Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }










////////////////////////////////////////////////////////
//	NEW CODED ADDED FOR ASSIGNMENT 2 FUNCTIONALITY    //
////////////////////////////////////////////////////////


public:
	//Traces from the camera
	bool Trace(
		UWorld* World,
		TArray<AActor*>& ActorsToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel,
		bool ReturnPhysMat
	);

	//Function that runs through each trace and finds actors 
	void CallMyTrace();

	//Processes the trace and returns the object hit
	void ProcessTraceHit(FHitResult& HitOut);

	//Clears all information about previous trace
	void ClearTraceInfo();

	//Function run every frame
	virtual void Tick(float DeltaTime) override;

	//Returns the name of the most previously traced item
	FString TraceName;

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Access to the character's current health
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetCurrentHealth();

	//Access to the character's max health
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetMaxHealth();

	//This updates the character's health
	//@param DeltaPower - This is the amount to change power by, can be position or negative
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Health")
		void UpdateHealth(float DeltaHealth);

	//Access to the character's light state
	UFUNCTION(BlueprintPure, Category = "Light")
		bool GetLightActive();

	//Set the character's light state
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Light")
		void SetLightActive(bool NewState);

	//Function to call when light is activated
	//A blueprint native event allows to put some functionality in blueprint and in C++
	UFUNCTION(BlueprintNativeEvent, Category = "Light")
		void WasToggled();
	virtual void WasToggled_Implementation();

	//Gets whether or not the fuse has been collected
	UFUNCTION(BlueprintPure, Category = "Fuse")
		bool GetFuseCollected();

	//Collect a fuse
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Fuse")
		void SetFuse();

	//Function to call when the fuse is collected
	UFUNCTION(BlueprintNativeEvent, Category = "Fuse")
		void FuseCollected();
	virtual void FuseCollected_Implementation();

	//Sets the new game state from HUD calls
	UFUNCTION(BlueprintCallable, Category = "State")
		void SetGameState(EGameState NewGameState);

	//Sets the new game state (from server), for HUD
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "State")
		void UpdateGameState(EGameState NewGameState);

	//Returns whether the character is a server or not
	UFUNCTION(BlueprintPure, Category = "Server")
		bool GetIsServer();

protected:
	//Stores the character's health (not editable anywhere else)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "Health")
		float CurrentHealth;

	//Stores the character's max health
	UPROPERTY(VisibleAnywhere, Category = "Health")
		float MaxHealth;

	//Health is updated on clients
	UFUNCTION()
		void OnRep_CurrentHealth();

	//Each player can collect 1 fuse
	UPROPERTY(ReplicatedUsing = OnRep_FuseCollected, VisibleAnywhere, Category = "Fuse")
		bool bFuseCollected;

	//When the fuse is collectted
	UFUNCTION()
		void OnRep_FuseCollected();

	//Determines whether the players light is on or off
	UPROPERTY(ReplicatedUsing = OnRep_LightActive, EditAnywhere, BlueprintReadWrite, Category = "Light", Meta = (BlueprintProtected = "true"))
		bool bLightActive;

	//Player's light is updated on client
	UFUNCTION()
		void OnRep_LightActive();

	//Server function for changing light
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerToggleLight();

	//True when the light is on, False when it is off
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
		AInteractable* CurrentInteractable;

	//Change the lamp state
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		void SetInteractable(AInteractable* NewInteractable);

	//Entry Point which is called when a player presses a key to collect some pickups
	UFUNCTION(BlueprintCallable, Category = "Action")
		void CheckAction();

	//Convention is to use Server in front of function to call code only on server
	//Reliable means that the function MUST be called on both server and client regardless of latency
	//Server means it will only run on an authority device
	//WithValidation will create two calls - one to validate and one to call implementation
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerCheckAction(AInteractable* object);

	//Sets the new game state to server
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetGameState(EGameState NewGameState);

	//Update state on clients
	UFUNCTION(NetMulticast, Reliable)
		void OnRep_SetGameState(EGameState NewGameState);

	//Stores whether the character is a server or client.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information")
		bool bIsServer;

private:
	//Client side handling of activating light
	//The multicast calls out to all servers and clients
	UFUNCTION(NetMulticast, Unreliable)
		void ClientOnToggle();

};

