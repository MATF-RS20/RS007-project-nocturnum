// Fill out your copyright notice in the Description page of Project Settings.

#include "HidableObject.h"


// Sets default values
AHidableObject::AHidableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall Mesh Component"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	UStaticMesh* Asset = MeshAsset.Object;
	ObjectMeshComponent->SetStaticMesh(Asset);


	ShouldHide = false;
	ShouldShow = false;
	FinalFadeAmount = 0.425f; // Minimum Opacity
	InitialFadeAmount = 1.0f;		// Maximum Opacity by default
	CurrentFade = InitialFadeAmount;
	FadeRate = 2.00f;		// Rate at which to fade between max and min opacity
	
	 // Ucitamo transparentni materijal
	static ConstructorHelpers::FObjectFinder<UMaterial> t_Mat(TEXT("Material'/Game/Materials/MDitheredDefault.MDitheredDefault'"));
	if (t_Mat.Object != NULL) {
		DitheredMaterial = t_Mat.Object;
	}
	// Ucitamo solidni materijal
	static ConstructorHelpers::FObjectFinder<UMaterial> o_Mat(TEXT("Material'/Game/Materials/MOpaqueDefault.MOpaqueDefault'"));
	if (o_Mat.Object != NULL) {
		OpaqueMaterial = o_Mat.Object;
	}
}

// Called when the game starts or when spawned
void AHidableObject::BeginPlay()
{
	Super::BeginPlay();
	// Podesavanje pocetnog materijala
	ObjectMeshComponent->SetMaterial(0, OpaqueMaterial);
	ObjectMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
}

// Called every frame
void AHidableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldHide) {
		CurrentFade -= DeltaTime * FadeRate;

		// If CurrentFade is less then our desired FinalFadeAmount, cap it off and tell this wall to stop hiding
		if (CurrentFade < FinalFadeAmount) {
			CurrentFade = FinalFadeAmount;
			ShouldHide = false;
		}
		// Set the translucent material's Opacity property to CurrentFade
		ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("DitherOpacity"), CurrentFade);


	}
	// If this wall should be showing, increment CurrentFade by our FadeRate
	else if (ShouldShow) {
		CurrentFade += DeltaTime * FadeRate;

		// If CurrentFade is more then 1.0 (maximum opacity), cap it off and tell this wall to stop showing
		// If you're using an opaque material for normal display purposes, change the material back now
		if (CurrentFade > 1.0f) {
			CurrentFade = 1.0f;
			ShouldShow = false;
			ChangeMeshMaterialToOpaque();
		}
		else {
			ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("DitherOpacity"), CurrentFade);
		}
	}
}

void AHidableObject::HideObject() {
	if (!ShouldHide) {
		ShouldShow = false;
		ShouldHide = true;
		ChangeMeshMaterialToDithered();
		ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("DitherOpacity"), CurrentFade);
	}
}

void AHidableObject::ShowObject() {
	if (!ShouldShow) {
		ShouldShow = true;
		ShouldHide = false;
	}
}

void AHidableObject::ChangeMeshMaterialToOpaque() {
	if (OpaqueMaterial) {
		ObjectMeshComponent->SetMaterial(0, OpaqueMaterial);
	}
}

void AHidableObject::ChangeMeshMaterialToDithered() {
	if (DitheredMaterial){
		ObjectMeshComponent->SetMaterial(0, DitheredMaterial);
	}
}