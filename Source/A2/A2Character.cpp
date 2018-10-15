// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "A2Character.h"
#include "A2Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Net/UnrealNetwork.h"
#include "A2GameMode.h"
#include "Door.h"
#include "FuseLock.h"
#include "A2HUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AA2Character

AA2Character::AA2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;






	///////////////////////////////////////////////////
	// NEW CODE ADDED FOR ASSIGNMENT 2 FUNCTIONALITY //
	///////////////////////////////////////////////////

	if (Role == ROLE_Authority) {
		CurrentInteractable = NULL;
	}

	MaxHealth = 100.f;

	//Starts the light as active
	SetLightActive(true);

	//Starts the fuse collected as false
	bFuseCollected = false;
}

void AA2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	//Set the starting health to max
	CurrentHealth = MaxHealth;

	//Check if this character is a server or client
	bIsServer = (Role == ROLE_Authority);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AA2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AA2Character::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AA2Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AA2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AA2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AA2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AA2Character::LookUpAtRate);

	//Input for button functionality
	InputComponent->BindAction("Action", IE_Pressed, this, &AA2Character::CheckAction);
	InputComponent->BindAction("Pause", IE_Pressed, this, &AA2Character::PauseGame);
}

void AA2Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AA2Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AA2Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AA2Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AA2Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AA2Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AA2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AA2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AA2Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AA2Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AA2Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AA2Character::TouchUpdate);
		return true;
	}
	
	return false;
}

















////////////////////////////////////////////////////////
//	NEW CODED ADDED FOR ASSIGNMENT 2 FUNCTIONALITY    //
////////////////////////////////////////////////////////

//Network code for saving variables
void AA2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AA2Character, CurrentHealth);
	DOREPLIFETIME(AA2Character, bLightActive);
	DOREPLIFETIME(AA2Character, bFuseCollected);
}

//Function called when the button is pressed for action
void AA2Character::CheckAction()
{
	ServerCheckAction(CurrentInteractable);
}

//This value is called before the implementation of the interactable function
//Checks for bad data or failed networking. In most cases, returns true
bool AA2Character::ServerCheckAction_Validate(AInteractable* object)
{
	return true;
}

//Implements code from checking actions
//NOTE: Both this and the function above are not defined in the header file as they are created from the base function
void AA2Character::ServerCheckAction_Implementation(AInteractable* object)
{
	if (Role == ROLE_Authority) {

		//Activates object
		if (object != NULL) {
			object->SetActive(!object->IsActive());
		}

		//Check if the object is an item
		if (AItem* item = Cast<AItem>(object)) {
			//Get the item type of the object
			EItemTypes itemType = item->GetItemType();

			//If it is a health item
			if (itemType == EItemTypes::Health) {
				float healthBoost = item->GetItemValue();

				//Increase health
				UpdateHealth(healthBoost);
			}
			//If it is a key item
			else if (itemType == EItemTypes::Key) {
				//Get keyId from the door
				int keyId = int(item->GetItemValue());

				//Call gamemode to unlock door
				if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
					gameMode->UnlockDoor(keyId);
				}
			}
			//If it is a fuse item
			else if (itemType == EItemTypes::Fuse) {
				//Set the fuse in player to true
				SetFuse();
			}
		}

		//Check if the object is a fuse lock
		if (AFuseLock* fuse = Cast<AFuseLock>(object)) {
			//If the current fuse has been collecte
			if (GetFuseCollected()) {
				//Tell the gamemode to update the fuse lock
				if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
					gameMode->UpdateFuse();

					//Remove fuse from player
					bFuseCollected = false;
					OnRep_FuseCollected();
				}
			}
		}
	}
}

//Sets the new interactable item
void AA2Character::SetInteractable(AInteractable* NewInteractable)
{
	CurrentInteractable = NewInteractable;
}

// Called every frame
void AA2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CallMyTrace();

}

//Runs the trace from the camera to the end distance
bool AA2Character::Trace(
	UWorld* World,
	TArray<AActor*>& ActorsToIgnore,
	const FVector& Start,
	const FVector& End,
	FHitResult& HitOut,
	ECollisionChannel CollisionChannel = ECC_Pawn,
	bool ReturnPhysMat = false
) {

	// The World parameter refers to our game world (map/level) 
	// If there is no World, abort
	if (!World)
	{
		return false;
	}

	// Set up our TraceParams object
	FCollisionQueryParams TraceParams(FName(TEXT("My Trace")), true, ActorsToIgnore[0]);

	// Should we simple or complex collision?
	TraceParams.bTraceComplex = true;

	// We don't need Physics materials 
	TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;

	// Add our ActorsToIgnore
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	// When we're debugging it is really useful to see where our trace is in the world
	// We can use World->DebugDrawTraceTag to tell Unreal to draw debug lines for our trace
	// (remove these lines to remove the debug - or better create a debug switch!)
	const FName TraceTag("MyTraceTag");
	World->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;

	// Force clear the HitData which contains our results
	HitOut = FHitResult(ForceInit);

	// Perform our trace
	World->LineTraceSingleByChannel
	(
		HitOut,				//result
		Start,				//start
		End,				//end
		CollisionChannel,	//collision channel
		TraceParams
	);

	// If we hit an actor, return true
	return (HitOut.GetActor() != NULL);
}


//CallMyTrace() - sets up our parameters and then calls our Trace() function
void AA2Character::CallMyTrace()
{
	// Get the location of the camera (where we are looking from) and the direction we are looking in
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();

	// How for in front of our character do we want our trace to extend?
	// ForwardVector is a unit vector, so we multiply by the desired distance
	const FVector End = Start + ForwardVector * 256;

	// Force clear the HitData which contains our results
	FHitResult HitData(ForceInit);

	// What Actors do we want our trace to Ignore?
	TArray<AActor*> ActorsToIgnore;

	//Ignore the player character - so you don't hit yourself!
	ActorsToIgnore.Add(this);

	// Call our Trace() function with the paramaters we have set up
	// If it Hits anything
	if (Trace(GetWorld(), ActorsToIgnore, Start, End, HitData, ECC_Visibility, false))
	{
		// Process our HitData
		if (HitData.GetActor())
		{
			//UE_LOG(LogClass, Warning, TEXT("This a testing statement. %s"), *HitData.GetActor()->GetName());
			ProcessTraceHit(HitData);
		}
		else
		{
			// The trace did not return an Actor or An error has occurred
			// Record a message in the error log
			ClearTraceInfo();
		}
	}
	else
	{
		// We did not hit an Actor
		ClearTraceInfo();
	}
}

//ProcessTraceHit() - process our Trace Hit result
void AA2Character::ProcessTraceHit(FHitResult& HitOut)
{
	ClearTraceInfo(); //Clears all current selected traces, to allow a new set of traced variables

	//Check to see if trace hit is a an interactable object
	if (AInteractable* const TestObject = Cast<AInteractable>(HitOut.GetActor()))
	{
		// Keep a pointer to the Item
		SetInteractable(TestObject);
		//CurrentItem = TestPickup;

		// Set a local variable of the PickupName for the HUD
		TraceName = TestObject->GetInteractableName();
	}
}

//Clears previous trace information
void AA2Character::ClearTraceInfo() {
	TraceName = "";
	SetInteractable(NULL);
}

//Gets the current health of the character
float AA2Character::GetCurrentHealth()
{
	return CurrentHealth;
}

//Gets the max health of the character
float AA2Character::GetMaxHealth()
{
	return MaxHealth;
}

//Updates the health of the character
void AA2Character::UpdateHealth(float DeltaHealth)
{
	if (Role == ROLE_Authority) {
		//Increase or decrease the current power
		CurrentHealth += DeltaHealth;

		//TODO
		//Set movement speed based on the health level?
		//GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CurrentPower;

		//Ensure that the current health is limited by the max health
		if (CurrentHealth > MaxHealth)
			CurrentHealth = MaxHealth;

		//Fake the rep notify as the listen server will not update automatically
		OnRep_CurrentHealth();
	}
}

//Called when the server calls a change in health
void AA2Character::OnRep_CurrentHealth()
{
	//UE_LOG(LogClass, Warning, TEXT("Set new health! %s"), *GetName());
	//Call a blueprint executable event maybe?
}

//When the fire button is pressed
void AA2Character::OnFire()
{
	ServerToggleLight();
}


//Gets the current light state
bool AA2Character::GetLightActive() {
	return bLightActive;
}

//Sets the current light state
void AA2Character::SetLightActive(bool NewState) {
	bLightActive = NewState;
	ClientOnToggle();
}

//Called when the server changes the light state
void AA2Character::OnRep_LightActive() {
	//Call a blueprint executable event
}

bool AA2Character::ServerToggleLight_Validate()
{
	return true;
}

//Implements code from checking actions
//NOTE: Both this and the function above are not defined in the header file as they are created from the base function
void AA2Character::ServerToggleLight_Implementation()
{
	if (Role == ROLE_Authority)
		SetLightActive(!bLightActive);
}

//Functionality when the light is toggled
void AA2Character::WasToggled_Implementation()
{
	UE_LOG(LogClass, Warning, TEXT("Change the light state %s"), *GetName());
}

//Add the _Implementation for the code behind the client toggled the light - MULTICAST event
void AA2Character::ClientOnToggle_Implementation()
{
	//Fire the Blueprint Native Event, which itself cannot be replicated
	WasToggled();
}

//Returns whether or not the fuse is collected
bool AA2Character::GetFuseCollected() {
	return bFuseCollected;
}

//Collects a fuse
void AA2Character::SetFuse() {
	if (Role == ROLE_Authority) {
		//Set the fuse to true
		bFuseCollected = true;

		//Fake the notify function for server
		OnRep_FuseCollected();

		UE_LOG(LogClass, Warning, TEXT("Test"));
	}
}

//Called when the fuse is updated
void AA2Character::OnRep_FuseCollected() {
	UE_LOG(LogClass, Warning, TEXT("Fuse was collected"));
}

//Functionality when the fuse is updated
void AA2Character::FuseCollected_Implementation() {
	
}

bool AA2Character::GetIsServer() {
	return bIsServer;
}

//Sets the pause or resume state (for HUD purposes)
void AA2Character::UpdateGameState(EGameState NewGameState) {
	if (Role == ROLE_Authority) {
		OnRep_SetGameState(NewGameState);
	}
}

//Implements the multicast call
void AA2Character::OnRep_SetGameState_Implementation(EGameState NewGameState)
{
	if (AA2HUD* HUD = Cast<AA2HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD())) {
		if (NewGameState == EGameState::Paused) {
			HUD->PauseGame();
		}
		else if (NewGameState == EGameState::Playing){
			HUD->ResumeGame();
		}
		else if (NewGameState == EGameState::Won) {
			HUD->WinGame();
		}
		else if (NewGameState == EGameState::Dead) {
			HUD->LossGame();
		}
	}

	//For the client (not server)
	if (Role != ROLE_Authority) {

		//Get game world
		UWorld* World = GetWorld();
		check(World);

		//Pause or Resume the game (as the game mode only did this on its own device)
		UGameplayStatics::SetGamePaused(World, true);
	}
}

bool AA2Character::ServerSetGameState_Validate(EGameState NewGameState)
{
	return true;
}

//Implements code from checking actions
void AA2Character::ServerSetGameState_Implementation(EGameState NewGameState)
{
	//Update the HUD game state
	if (Role == ROLE_Authority) {
		if (AA2GameMode* const gameMode = Cast<AA2GameMode>(GetWorld()->GetAuthGameMode())) {
			gameMode->SetGameState(NewGameState);
		}
	}
}

//Calls HUD to set new game state
void AA2Character::SetGameState(EGameState NewGameState) {
	ServerSetGameState(NewGameState);
}


//Pauses the game from character controls
void AA2Character::PauseGame() {
	ServerSetGameState(EGameState::Paused);
}