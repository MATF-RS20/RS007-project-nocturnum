// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Classes/GameFramework/SpringArmComponent.h"
#include "Classes/GameFramework/PlayerController.h"
#include "Classes/Camera/CameraComponent.h"
#include "Containers/Queue.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"


UCLASS()
class NOCTURNUM_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
		USceneComponent* RootScene;

	UPROPERTY()
		USpringArmComponent* SpringArm;

	UPROPERTY()
		UCameraComponent* CameraComp;

	UPROPERTY()
		APlayerController* PC;

	UFUNCTION()
		FVector GetCameraPanDirection();

	UPROPERTY()
		float PaningMargin = 15;
		int32 ScreenSizeX;
		int32 ScreenSizeY;
		float CamSpeed = 40;
		float CameraLagSpeed = 3.0f;
		float MaxLagDistance = 30;
		float MaxZoom = 1000;
		float MinZoom = 50;
		float ZoomAmount = 200;
		float DesiredArmLenght;
		float InitialArmLength;

		float AngleIncrement = 45;
		FRotator CurrentRotation;
		float RotationFactor;
		FVector2D CameraInput;
		TQueue<int> RotationQueue;
		int NextRotation = 0;
		FRotator InitialRotation;
		bool ResettingRotation = false;
		float Stamp;


		void ZoomIn();
		void ZoomOut();

		void SetDesiredArmLenght(bool isZoomingOut);

		void RotateLeft();
		void RotateRight();
		void ResetCamera();
};
