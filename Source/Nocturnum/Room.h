// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Room.generated.h"

UCLASS()
class NOCTURNUM_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
		Sve sledece bool vrednosti nam govore da li se 
		nalazi ulaz u prostoriju na da toj strani.
		Svi Blueprinti koji su napravljeni od ove klase moraju
		imati cekirana odgovarajuca polja.
		Istok je negativno X, zapad pozitivno X, sever pozitivno Y,
		jug negativno Y.
	*/

	// Ulaz na +Y ?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
		bool north = false;
	// Ulaz na -X ?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
		bool east = false;
	// Ulaz na -Y ?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
		bool south = false;
	// Ulaz na +X ?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Connection")
		bool west = false;
	//Da li je prostorija prethodno aktivirana.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay")
		bool previouslyActivated = false;

	//Akcije koje ce se izvrsiti kada udjemo u prostoriju
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void activeRoom();
	//Da mozemo u BP da napravimo ovaj event.
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
		void activeRoomBP();
	//Lik kog spawnujemo.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay")
		TSubclassOf<ACharacter> spawnEnemy = NULL;

};
