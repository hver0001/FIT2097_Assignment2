// Fill out your copyright notice in the Description page of Project Settings.

#include "EndBox.h"
#include "A2Character.h"
#include "A2GameMode.h"


// Sets default values
AEndBox::AEndBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the box trigger as a box component
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	BoxTrigger->AttachTo(RootComponent);

}

// Called when the game starts or when spawned
void AEndBox::BeginPlay()
{
	Super::BeginPlay();
	
	//Ensure the box trigger exists and has been created
	if (BoxTrigger) {
		//Create the overlap event and connect it to a function
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEndBox::OnBeginOverlap);
		BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AEndBox::OnEndOverlap);
	}

}

// Called every frame
void AEndBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Called when a new object enters the trigger
void AEndBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (Role == ROLE_Authority) {
		//Check if the collided actor is a character
		if (AA2Character* Character = Cast<AA2Character>(OtherActor)) {
			UE_LOG(LogClass, Warning, TEXT("NEW CHARACTER HAS ENTERED THE TRIGGER"));
			//Tell the game mode to increase player count
			if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
				gameMode->SetEndPlayerCount(1);
			}
		}
	}
}

//Called when an object leaves the trigger
void AEndBox::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Role == ROLE_Authority) {
		//Check if the left actor is a character
		if (AA2Character* Character = Cast<AA2Character>(OtherActor)) {
			UE_LOG(LogClass, Warning, TEXT("NEW CHARACTER HAS Left THE TRIGGER"));
			//Tell the game mode to decrease player count
			if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
				gameMode->SetEndPlayerCount(-1);
			}
		}
	}
}

