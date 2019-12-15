// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Engine.h"
#include <cstdlib>
#include <iostream>

// Sets default values
ACameraPawn::ACameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootScene);
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-50, 0, 0));
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = CameraLagSpeed;
	SpringArm->CameraLagMaxDistance = MaxLagDistance;
	CurrentRotation = SpringArm->GetComponentRotation();
	InitialRotation = SpringArm->GetComponentRotation();

	DesiredArmLenght = SpringArm->TargetArmLength;
	InitialArmLength = SpringArm->TargetArmLength;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());
	PC->GetViewportSize(ScreenSizeX, ScreenSizeY);
}
//PRINTING
/* 
if (GEngine) {
	FString TheFloatStr = FString::SanitizeFloat(SpringArm->GetComponentRotation().Yaw);
	GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Yellow, *TheFloatStr);
}
*/

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorWorldOffset(GetCameraPanDirection() * CamSpeed);

	if (!FMath::IsNearlyEqual(SpringArm->TargetArmLength, DesiredArmLenght, 0.5f)) {
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredArmLenght, DeltaTime, 5.0);
	}
	
	if (ResettingRotation == false) {
		if (RotationQueue.IsEmpty() == false) {
			RotationQueue.Peek(NextRotation);
			if (NextRotation == 1) {
				RotationFactor += DeltaTime / 0.15;
				float v = FMath::Lerp<float>(0.0, AngleIncrement, RotationFactor);
				if (v < AngleIncrement) {
					CurrentRotation.Add(0.0, v, 0.0);
					SpringArm->SetRelativeRotation(CurrentRotation);
					CurrentRotation.Add(0.0, -v, 0.0);
				}

				else {
					CurrentRotation.Add(0.0, AngleIncrement, 0.0);
					SpringArm->SetRelativeRotation(CurrentRotation);
					CurrentRotation = SpringArm->GetComponentRotation();
					RotationQueue.Pop();
					NextRotation = 0;
					RotationFactor = 0;
				}
			}

			if (NextRotation == -1) {
				RotationFactor += DeltaTime / 0.15;
				float v = FMath::Lerp<float>(0.0, AngleIncrement, RotationFactor);
				if (v < AngleIncrement) {
					CurrentRotation.Add(0.0, -v, 0.0);
					SpringArm->SetRelativeRotation(CurrentRotation);
					CurrentRotation.Add(0.0, +v, 0.0);
				}

				else {
					CurrentRotation.Add(0.0, -AngleIncrement, 0.0);
					SpringArm->SetRelativeRotation(CurrentRotation);
					CurrentRotation = SpringArm->GetComponentRotation();
					RotationQueue.Pop();
					NextRotation = 0;
					RotationFactor = 0;
				}
			}
		}
	}
	else {
		RotationFactor += DeltaTime / 0.15;
		float v = FMath::Lerp<float>(Stamp, InitialRotation.Yaw, RotationFactor);
		if (!FMath::IsNearlyEqual(InitialRotation.Yaw, v, 9.0f)) {
			CurrentRotation.Yaw = v;
			SpringArm->SetRelativeRotation(CurrentRotation);
		}
		else {
			SpringArm->SetRelativeRotation(InitialRotation);
			CurrentRotation = SpringArm->GetComponentRotation();
			ResettingRotation = false;
			RotationFactor = 0;
		}
	}
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACameraPawn::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACameraPawn::ZoomOut);
	InputComponent->BindAction("RotateLeft", IE_Pressed, this, &ACameraPawn::RotateLeft);
	InputComponent->BindAction("RotateRight", IE_Pressed, this, &ACameraPawn::RotateRight);
	InputComponent->BindAction("ResetCamera", IE_Pressed, this, &ACameraPawn::ResetCamera);
}

void ACameraPawn::ZoomIn() {
	SetDesiredArmLenght(false);
}
void ACameraPawn::ZoomOut() {
	SetDesiredArmLenght(true);
}

void ACameraPawn::SetDesiredArmLenght(bool isZoomingOut) {
	DesiredArmLenght = isZoomingOut ? SpringArm->TargetArmLength + ZoomAmount :
		SpringArm->TargetArmLength - ZoomAmount;
	if (DesiredArmLenght > MaxZoom || DesiredArmLenght < MinZoom) {
		DesiredArmLenght = FMath::Min<float>(FMath::Max(DesiredArmLenght, MinZoom), MaxZoom);
	}
}

void ACameraPawn::RotateLeft() {
	RotationQueue.Enqueue(1);
}
void ACameraPawn::RotateRight() {
	RotationQueue.Enqueue(-1);
}
void ACameraPawn::ResetCamera() {
	NextRotation = 0;
	RotationQueue.Empty();
	CurrentRotation = SpringArm->GetComponentRotation();
	Stamp = CurrentRotation.Yaw;
	ResettingRotation = true;
	DesiredArmLenght = InitialArmLength;

}
FVector ACameraPawn::GetCameraPanDirection()
{
	float MousePositionX;
	float MousePositionY;
	float CamDirectionX = 0;
	float CamDirectionY = 0;
	PC->GetMousePosition(MousePositionX, MousePositionY);

	if (MousePositionX <= PaningMargin) {
		CamDirectionY = -1;
	}

	if (MousePositionY <= PaningMargin) {
		CamDirectionX = 1;
	}

	if (MousePositionX >= ScreenSizeX - PaningMargin) {
		CamDirectionY = 1;
	}

	if (MousePositionY >= ScreenSizeY - PaningMargin) {
		CamDirectionX = -1;
	}

	if (CamDirectionX != 0 && CamDirectionY != 0) {
		CamDirectionX /= 1.41421356237;
		CamDirectionY /= 1.41421356237;
	}

	FRotator CurrentDirectionRotation = SpringArm->GetComponentRotation();
	return FVector(CamDirectionX, CamDirectionY, 0).RotateAngleAxis(CurrentDirectionRotation.Yaw, FVector(0.0, 0.0, 1.0));
}

