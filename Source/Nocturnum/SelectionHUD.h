// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NocturnumCharacter.h"
#include "EngineUtils.h"
#include "SelectionHUD.generated.h"


UCLASS()
class NOCTURNUM_API ASelectionHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	void DrawHealthBars() const;
	UPROPERTY()
		FVector2D InitialPoint;
	UPROPERTY()
		FVector2D CurrentPoint;

	UFUNCTION()
		FVector2D GetMousePosition();

	UPROPERTY()
		bool bStartSelecting = false;
	UPROPERTY()
		TArray <ANocturnumCharacter*> SelectedActors;
	UPROPERTY()
		TArray <AActor*> AllActors;

		
};
