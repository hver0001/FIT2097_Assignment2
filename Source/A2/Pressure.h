// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Pressure.generated.h"

UCLASS()
class A2_API APressure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Function is called when the pressure is collided with a new object
	UFUNCTION(BlueprintCallable, Category = "Pressure")
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	
	//Function is called when the pressure ends collision with new object
	UFUNCTION(BlueprintCallable, Category = "Pressure")
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Sets the box component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pressure", Meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxTrigger;

	//Returns the box component
	FORCEINLINE class UBoxComponent* GetBoxTrigger() const { return BoxTrigger; }

	//This function is required for networking
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//This function returns whether or not the pressure plate is interacted with
	UFUNCTION(BlueprintPure, Category = "Pressure")
		bool IsActive();

	//Mutator for the activation of an object
	UFUNCTION(BlueprintCallable, Category = "Pressure")
		virtual void SetActive(bool NewState);

	//Function to call when pressure plate is activated
	UFUNCTION(BlueprintNativeEvent, Category = "Pressure")
		void WasToggled();
	virtual void WasToggled_Implementation();

protected:
	//Stores whether or not the pressure plate has been activated or not
	UPROPERTY(ReplicatedUsing = OnRep_IsActive, EditAnywhere, BlueprintReadWrite, Category = "Pressure", Meta = (BlueprintProtected = "true"))
		bool bIsActive;

	//Called whenever IsActive is updated
	UFUNCTION()
		virtual void OnRep_IsActive();

private:
	//Client side handling of being picked up
	UFUNCTION(NetMulticast, Unreliable)
		void ClientOnToggle();
	
};
