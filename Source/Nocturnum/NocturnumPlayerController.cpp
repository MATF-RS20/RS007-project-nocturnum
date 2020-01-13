// Fill out your copyright notice in the Description page of Project Settings.


#include "NocturnumPlayerController.h"
#include "AI/Navigation//NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "NocturnumEnemyBot.h"
#include "DrawDebugHelpers.h"
#include "Bullet.h"
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

	InputComponent->BindAction("Shoot", IE_Pressed, this, &ANocturnumPlayerController::Shoot);
	InputComponent->BindAction("CenterCameraOnActor", IE_Pressed, this, &ANocturnumPlayerController::CenterCameraOnActor);
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
			ANocturnumEnemyBot* Bot = Cast<ANocturnumEnemyBot>(Hit.Actor.Get());
			if (Bot && FVector::Distance(Selected[0]->GetActorLocation(), Bot->GetActorLocation()) < WeaponFiringRange) {
				FRotator CurrentRotation = Selected[0]->GetActorRotation();
				FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(Selected[0]->GetActorLocation(), Bot->GetActorLocation());
				Selected[0]->SetActorRotation(FRotator(CurrentRotation.Pitch, NewRotation.Yaw, CurrentRotation.Roll));
				Shoot();
			}
			else {
				FVector MoveLocation = Hit.Location + FVector(i / 2 * 100, i % 2 * 100, 0);
				UNavigationSystem::SimpleMoveToLocation(Selected[i]->GetController(), MoveLocation);
				DrawDebugCircle(GetWorld(), MoveLocation, 30.0, 100, FColor::Red, true, 1, (uint8)'\000', 8, FVector(1, 0, 0), FVector(0, 1, 0));
			}
		}
	}
}

void ANocturnumPlayerController::Shoot()
{
	if (Selected.Num() > 0) {
		FVector StartingLocation = Selected[0]->GetActorLocation();
		FVector ForwardVector = Selected[0]->GetActorForwardVector();
		FRotator CurrentRotation = Selected[0]->GetActorRotation();

		StartingLocation += ForwardVector * 100;

		ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(ABullet::StaticClass(), StartingLocation, CurrentRotation + FRotator(90.0f, 0.0, 180.0f));
		Bullet->SetMovementDirection(ForwardVector);
		Bullet->SetActorEnableCollision(false);
	}
}

void ANocturnumPlayerController::CenterCameraOnActor()
{
	if (Selected.Num() > 0) {
		FVector ActorLocation = Selected[0]->GetActorLocation();
		FVector CurrentCameraLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		FVector Destination = FVector(ActorLocation.X - 500.0f, ActorLocation.Y, CurrentCameraLocation.Z);
		GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(Destination);	
	}
}
