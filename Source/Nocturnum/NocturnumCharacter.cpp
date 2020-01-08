// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "NocturnumCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"

ANocturnumCharacter::ANocturnumCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	CursorToWorld->SetVisibility(false);


	//Create a selection ring decal
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("Selection Decal"));
	SelectionDecal->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterial(TEXT("Material'/Game/TopDownCPP/Blueprints/SelectionRingDecal.SelectionRingDecal'"));
	if (DecalMaterial.Succeeded()) {
		SelectionDecal->SetDecalMaterial(DecalMaterial.Object);

		// Izracunatri parametri
		SelectionDecal->RelativeLocation = FVector(0.f, 0.f, -80.f);
		SelectionDecal->DecalSize = FVector(32.f, 64.f, 64.f);
		SelectionDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		SelectionDecal->SetVisibility(false);
	}


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Health parameters
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	HealthRegenRate = 0.0f;
}

void ANocturnumCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	DetectBlockingObjects();

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

void ANocturnumCharacter::SetSelected() {
	//CursorToWorld->SetVisibility(false);
	SelectionDecal->SetVisibility(true);
}

void ANocturnumCharacter::SetDeselected() {
	//CursorToWorld->SetVisibility(false);
	SelectionDecal->SetVisibility(false);
}

float ANocturnumCharacter::GetMaxHealth()
{
	return MaxHealth;
}

float ANocturnumCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}


void ANocturnumCharacter::DetectBlockingObjects() {
	FVector start = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	float ScaleParamZ = this->GetActorScale().Z * 80.0f;
	float ScaleParamY = this->GetActorScale().Y * 40.0f;
	// Middle section
	FVector HeadMid = this->GetActorLocation() + FVector(0.0f, 0.0f, ScaleParamZ); // Aiming for the head
	FVector TorsoMid = this->GetActorLocation(); // Aiming for the middle
	FVector LegsMid = this->GetActorLocation() + FVector(0.0f, 0.0f, -ScaleParamZ); // Aiming for the feet

	// Left section
	FVector HeadLeft = this->GetActorLocation() + FVector(0.0f, ScaleParamY, ScaleParamZ); // Aiming for the head
	FVector TorsoLeft = this->GetActorLocation(); // Aiming for the middle
	FVector LegsLeft = this->GetActorLocation() + FVector(0.0f, ScaleParamY, -ScaleParamZ); // Aiming for the feet

	// Right section
	FVector HeadRight = this->GetActorLocation() + FVector(0.0f, -ScaleParamY, ScaleParamZ); // Aiming for the head
	FVector TorsoRight = this->GetActorLocation(); // Aiming for the middle
	FVector LegsRight = this->GetActorLocation() + FVector(0.0f, -ScaleParamY, -ScaleParamZ); // Aiming for the feet

	// An array for storing objects that we hit, and an array for storing our fading objects 
	TArray<FHitResult> Hits1;
	TArray<FHitResult> Hits2;
	TArray<FHitResult> Hits3;
	TArray<FHitResult> Hits4;
	TArray<FHitResult> Hits5;
	TArray<FHitResult> Hits6;
	TArray<FHitResult> Hits7;
	TArray<FHitResult> Hits8;
	TArray<FHitResult> Hits9;
	TArray<AHidableObject*> ObjectsHitInThisPass;

	// Cast rays
	GetWorld()->LineTraceMultiByObjectType(Hits1, start, HeadMid, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits2, start, TorsoMid, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits3, start, LegsMid, FCollisionObjectQueryParams(ECC_WorldDynamic));

	GetWorld()->LineTraceMultiByObjectType(Hits4, start, HeadLeft, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits5, start, TorsoLeft, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits6, start, LegsLeft, FCollisionObjectQueryParams(ECC_WorldDynamic));

	GetWorld()->LineTraceMultiByObjectType(Hits7, start, HeadRight, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits8, start, TorsoRight, FCollisionObjectQueryParams(ECC_WorldDynamic));
	GetWorld()->LineTraceMultiByObjectType(Hits9, start, LegsRight, FCollisionObjectQueryParams(ECC_WorldDynamic));

	TArray<AHidableObject*> Objects;
	AddHitsToObjectList(Hits1, Objects);
	AddHitsToObjectList(Hits2, Objects);
	AddHitsToObjectList(Hits3, Objects);

	AddHitsToObjectList(Hits4, Objects);
	AddHitsToObjectList(Hits5, Objects);
	AddHitsToObjectList(Hits6, Objects);

	AddHitsToObjectList(Hits7, Objects);
	AddHitsToObjectList(Hits8, Objects);
	AddHitsToObjectList(Hits9, Objects);

	for (AHidableObject* Obj : Objects) {
		Obj->HideObject();
		ObjectsHitInThisPass.AddUnique(Obj);
		BlockingObjects.AddUnique(Obj);
	}

	TArray<AHidableObject*> ObjectsToRemove;
	for (AHidableObject* Obj : BlockingObjects) {
		if (!ObjectsHitInThisPass.Contains(Obj)) {
			Obj->ShowObject();
			ObjectsToRemove.Add(Obj);
		}
	}

	for (AHidableObject* Obj : ObjectsToRemove) {
		ObjectsToRemove.Remove(Obj);
	}

}

void ANocturnumCharacter::AddHitsToObjectList(const TArray<FHitResult>& HitList, TArray<AHidableObject*>& Objects)
{
	if (HitList.Num() > 0) {
		for (FHitResult Hit : HitList) {
			if (Hit.bBlockingHit) {
				AHidableObject* Obj = Cast<AHidableObject>(Hit.Actor.Get());
				if (Obj) {
					Objects.AddUnique(Obj);
				}
			}
		}
	}
}



