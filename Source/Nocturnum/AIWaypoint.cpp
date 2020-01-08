// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWaypoint.h"
#include "NocturnumEnemyBot.h"


// Sets default values
AAIWaypoint::AAIWaypoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(Root);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAIWaypoint::OnPlayerEnter);

}

// Called when the game starts or when spawned
void AAIWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIWaypoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIWaypoint::OnPlayerEnter(UPrimitiveComponent * OverlapComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, 
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ANocturnumEnemyBot* Bot = nullptr;
	if (OtherActor != nullptr) {
		Bot = Cast<ANocturnumEnemyBot>(OtherActor);
		if (Bot != nullptr) {
			Bot->NextWaypoint = NextWaypoint;
		}
	}
}

