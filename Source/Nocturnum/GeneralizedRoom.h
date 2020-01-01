// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "GeneralizedRoom.generated.h"

/**
 * 
 */
UCLASS()
class NOCTURNUM_API AGeneralizedRoom : public ARoom
{
	GENERATED_BODY()
	
public:

	//Poziva se pri pocetku igre
	virtual void BeginPlay() override;
	
	//da li se moze doci do sobe
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		bool isAccessible = false;

	//da li je trenutni cilj sobe u izvrsavanju
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		bool isObjectiveActive = false;

	//da li je trenutni cilj sobe zavrsen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		bool isObjectiveComplete = false;

	//kotisi se za informacije koje cemo prikazati na ekranu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		int roomId = 0;

	/*
			Celije koje ce ova soba zauzimati na osnovu lokalnih kordinata.
			(0m,0m) je (0,0) u blueprintima (30m,-30m) je (1,-1), celije su 30x30.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		TArray<FVector2D> occupiedCells;

	// Koordinate ove celije su koordinate ulaza u prostoriju.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FVector2D entranceCell;

	// Koordinate ove celije su koordinate izlaza iz prostorije.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FVector2D exitCell;
	// predstavlja vektor paralelan +-X ili +-Y, da bi generator znao odakle da pridje celiji
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FVector2D entranceDirection;

	// predstavlja vektor paralelan +-X ili +-Y, da bi generator znao odakle da izadje iz celije
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Connection")
		FVector2D exitDirection;
	
	//backup funkcija ako Blueprint ne uspe da procita iz nad klase.
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
		void activeRoomBP();

	// lokacija cilja za interfejs, Globalne koordinate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface")
		FVector objectiveLocation = FVector(0, 0, 0);
};
