// Fill out your copyright notice in the Description page of Project Settings.

#include "Room.h"
#include "Engine.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoom::activeRoom()
{
	if (previouslyActivated) return;
	else previouslyActivated = true;

	UWorld* const World = GetWorld();
	if (World) {
		if (spawnEnemy) {
			FTransform t = FTransform(GetTransform());
			t.AddToTranslation(FVector(100, 100, 100));
			ACharacter* spawnedChar = World->SpawnActor<ACharacter>(spawnEnemy->GetClass(),t);
			if (spawnedChar == NULL) {
				if (GEngine) {
					GEngine->AddOnScreenDebugMessage(-1, 5.f,
						FColor::Blue,
						TEXT("Neuspelo spawnovanje."));
				}
			}
		}
		else if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f,
				FColor::Blue,
				TEXT("spawnEnemy is NULL"));
		}
	}
	else if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f,
			FColor::Blue,
			TEXT("World je NULL"));
	}
}