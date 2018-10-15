// Fill out your copyright notice in the Description page of Project Settings.

#include "Pressure.h"
#include "A2Character.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APressure::APressure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the box trigger as a box component
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxTrigger->AttachTo(RootComponent);

	//Ensure the object replicates between the two devices
	bReplicates = true;

}

// Called when the game starts or when spawned
void APressure::BeginPlay()
{
	Super::BeginPlay();
	
	//Ensure the box trigger exists and has been created
	if (BoxTrigger) {
		//Create the overlap event and connect it to a function
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &APressure::OnBeginOverlap);
		BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &APressure::OnEndOverlap);
	}
}

// Called every frame
void APressure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Called when a new object enters the trigger
void APressure::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	//Check if the collided actor is a character
	if (AA2Character* Character = Cast<AA2Character>(OtherActor)) {
		UE_LOG(LogClass, Warning, TEXT("NEW CHARACTER HAS ENTERED THE TRIGGER"));
		SetActive(true);
	}
}

//Called when an object leaves the trigger
void APressure::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Check if the left actor is a character
	if (AA2Character* Character = Cast<AA2Character>(OtherActor)) {
		UE_LOG(LogClass, Warning, TEXT("NEW CHARACTER HAS Left THE TRIGGER"));
		SetActive(false);
	}
}

//Networking function for storing variables
void APressure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Allow the active variable to be replicated
	DOREPLIFETIME(APressure, bIsActive);
}

//Returns the state of the pressure plate
bool APressure::IsActive()
{
	return bIsActive;
}

//Sets a new state of the pressure plate
void APressure::SetActive(bool NewState)
{
	//Ensure only the server can set activation state
	if (Role == ROLE_Authority) {
		bIsActive = NewState;
		ClientOnToggle();
	}
}

//This is called whenever the active boolean is updated
void APressure::OnRep_IsActive()
{

}

//Functionality when the pressure plate is toggled
void APressure::WasToggled_Implementation()
{
	
}

//Add the _Implementation for the code behind the client toggel function - MULTICAST event
void APressure::ClientOnToggle_Implementation()
{
	//Fire the Blueprint Native Event, which itself cannot be replicated
	WasToggled();
}
