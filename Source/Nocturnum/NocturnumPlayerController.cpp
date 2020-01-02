// Fill out your copyright notice in the Description page of Project Settings.


#include "NocturnumPlayerController.h"
#include "AI/Navigation//NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"

ANocturnumPlayerController::ANocturnumPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ANocturnumPlayerController::BeginPlay()
{
	HUDPtr = Cast<ASelectionHUD>(GetHUD());
}

void ANocturnumPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouseClick", IE_Pressed, this, &ANocturnumPlayerController::SelectionPressed);
	InputComponent->BindAction("LeftMouseClick", IE_Released, this, &ANocturnumPlayerController::SelectionReleased);

	InputComponent->BindAction("RightMouseClick", IE_Released, this, &ANocturnumPlayerController::MoveReleased);
}

void ANocturnumPlayerController::SelectionPressed() {
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition();
	HUDPtr->bStartSelecting = true;

}

void ANocturnumPlayerController::SelectionReleased() {
	HUDPtr->bStartSelecting = false;
	Selected = HUDPtr->SelectedActors;
}

void ANocturnumPlayerController::MoveReleased() {
	if (Selected.Num() > 0) {
		for (int32 i = 0; i < Selected.Num(); i++)
		{
			FHitResult Hit;
			GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
			FVector MoveLocation = Hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0);
			UNavigationSystem::SimpleMoveToLocation(Selected[i]->GetController(), MoveLocation);
			//DrawDebugCircle(GetWorld(), MoveLocation, FVector(1, 0, 0), FVector(0, 1, 0), FColor::Blue, 25, 200, false, 2, (uint8)'\000', 8);
			DrawDebugCircle(GetWorld(), MoveLocation, 30.0, 100, FColor::Red, true, 1, (uint8)'\000', 8, FVector(1, 0, 0), FVector(0, 1, 0));
		}
	}
}