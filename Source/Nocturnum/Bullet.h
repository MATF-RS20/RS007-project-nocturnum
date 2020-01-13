// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" 
#include "Materials/Material.h" 
#include "Materials/MaterialInstanceDynamic.h" 
#include "Kismet/KismetMaterialLibrary.h"
#include "Bullet.generated.h"

UCLASS()
class NOCTURNUM_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

	UPROPERTY(EditAnywhere, Category = Display)
		float BulletSpeed;
	UPROPERTY(EditAnywhere, Category = Material)
		UMaterial* BulletMaterial;
	UPROPERTY()
		FVector MovementDirection;
	UFUNCTION()
		void SetMovementDirection(const FVector MovementVector);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ObjectMeshComponent;
	
};
