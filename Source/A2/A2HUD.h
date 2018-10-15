// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "A2HUD.generated.h"

UCLASS()
class AA2HUD : public AHUD
{
	GENERATED_BODY()

public:
	AA2HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	//Pauses the game
	UFUNCTION(BlueprintNativeEvent, Category = "State")
		void PauseGame();
	virtual void PauseGame_Implementation();

	//Resumes the game
	UFUNCTION(BlueprintNativeEvent, Category = "State")
		void ResumeGame();
	virtual void ResumeGame_Implementation();

	//Loads win screen
	UFUNCTION(BlueprintNativeEvent, Category = "State")
		void WinGame();
	virtual void WinGame_Implementation();

	//Loads loss screen
	UFUNCTION(BlueprintNativeEvent, Category = "State")
		void LossGame();
	virtual void LossGame_Implementation();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

