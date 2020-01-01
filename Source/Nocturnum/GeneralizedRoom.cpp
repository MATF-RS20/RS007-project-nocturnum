// Fill out your copyright notice in the Description page of Project Settings.

#include "GeneralizedRoom.h"


void AGeneralizedRoom::BeginPlay()
{
	Super::BeginPlay();

	//Proveri da li su validni ulazi i izlaz.
	for (FVector2D coord : occupiedCells) {
		FVector2D testEntry = entranceCell + entranceDirection;
		FVector2D testExit = exitCell + exitDirection;
		verify(!(coord.Equals(testEntry) || coord.Equals(testExit)));
	}
}

