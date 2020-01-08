// Fill out your copyright notice in the Description page of Project Settings.

#include "NocturnumEnemyBot.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANocturnumEnemyBot::ANocturnumEnemyBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	NextWaypoint = nullptr;
}

// Called when the game starts or when spawned
void ANocturnumEnemyBot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANocturnumEnemyBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANocturnumEnemyBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}