// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"


// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Dodelimo 3d model objektu
	ObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/Bullet.Bullet'"));
	UStaticMesh* Asset = MeshAsset.Object;
	ObjectMeshComponent->SetStaticMesh(Asset);

	BulletSpeed = 5000.0f;
}

void ABullet::SetMovementDirection(const FVector MovementVector)
{
	MovementDirection = MovementVector;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Kretanje
	SetActorLocation(GetActorLocation() + MovementDirection * BulletSpeed * DeltaTime);
}

