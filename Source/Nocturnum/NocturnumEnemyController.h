// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NocturnumEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class NOCTURNUM_API ANocturnumEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	ANocturnumEnemyController();
	
	virtual void BeginPlay() override;

	virtual void Possess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual FRotator GetControlRotation() const override;

	UFUNCTION()
		void OnPawnDetected(TArray<AActor*> DetectedPawns);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		float AISightRadius = 2000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		float AISightAge = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		float AIFieldOfView = 90.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		float AILoseSightRadius = AISightRadius + 250.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		bool bIsPlayerDetected = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		float PlayerDistance = 0.0f;
};
