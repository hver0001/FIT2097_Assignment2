// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Interactable.h"
#include "Item.generated.h"

//Creates an enumerator for the item
UENUM(BlueprintType) //BlueprintType
enum class EItemTypes : uint8 {
	Health UMETA(DisplayName = "Health Item"),
	Key UMETA(DisplayName = "Door Key")
};

/**
 * 
 */
UCLASS()
class A2_API AItem : public AInteractable
{
	GENERATED_BODY()
	
public:
	AItem();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//This allows other classes to safely change the objects's activation state
	UFUNCTION(BlueprintCallable, Category = "Interactable")
		void SetActive(bool NewState) override;

	//Get the item type associated
	UFUNCTION(BlueprintPure, Category = "Item")
		EItemTypes GetItemType();

	//Get the item value associated with the item
	UFUNCTION(BlueprintPure, Category = "Item")
		float GetItemValue();


protected:
	//The type of item being selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", Meta = (BlueprintProtected = "true"))
		EItemTypes ItemType;

	//This holds the value associated with the particular item. In the case of health pickup, it holds the health retrieved
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Item", Meta = (BlueprintProtected = "true"))
		float ItemValue;

	
	
};
