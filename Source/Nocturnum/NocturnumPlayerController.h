// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SelectionHUD.h"
#include "NocturnumPlayerController.generated.h"

UCLASS()
class NOCTURNUM_API ANocturnumPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANocturnumPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	ASelectionHUD* HUDPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> HealthBar;

	UUserWidget* HealthBarPtr;

protected:
	UFUNCTION()
		void SelectionPressed();
	UFUNCTION()
		void SelectionReleased();
	UFUNCTION()
		void MoveReleased();
	
	UPROPERTY()
		TArray <ANocturnumCharacter*> Selected;
};


