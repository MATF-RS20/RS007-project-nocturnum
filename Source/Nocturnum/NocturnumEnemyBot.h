// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIWaypoint.h"
#include "GameFramework/Character.h"
#include "NocturnumEnemyBot.generated.h"

UCLASS()
class NOCTURNUM_API ANocturnumEnemyBot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANocturnumEnemyBot();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		AAIWaypoint* NextWaypoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
