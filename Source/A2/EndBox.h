// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EndBox.generated.h"

UCLASS()
class A2_API AEndBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndBox();

	//Function is called when the pressure is collided with a new object
	UFUNCTION(BlueprintCallable, Category = "Box")
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	//Function is called when the pressure ends collision with new object
	UFUNCTION(BlueprintCallable, Category = "Box")
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//Sets the box component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Box", Meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxTrigger;

	//Returns the box component
	FORCEINLINE class UBoxComponent* GetBoxTrigger() const { return BoxTrigger; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
