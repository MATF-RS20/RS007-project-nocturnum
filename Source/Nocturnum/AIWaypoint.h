// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "AIWaypoint.generated.h"

UCLASS()
class NOCTURNUM_API AAIWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIWaypoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent*  CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AAIWaypoint* NextWaypoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnPlayerEnter(UPrimitiveComponent* OverlapComponent,
							AActor* OtherActor,
							UPrimitiveComponent* OtherComponent,
							int32 OtherBodyIndex,
							bool bFromSweep,
							const FHitResult &SweepResult);
	
};
