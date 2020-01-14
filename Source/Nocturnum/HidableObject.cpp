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
	FinalFadeAmount = 0.425f; // Minimalna vidljivost
	InitialFadeAmount = 1.0f;		// Maksimalna vidljivost
	CurrentFade = InitialFadeAmount;
	FadeRate = 2.00f;		// Brzina promene vidljivosti
	
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

		// Ako je trenutna vidljivost manja od minimalne, odseci je i reci objektu da prestane da se krije
		if (CurrentFade < FinalFadeAmount) {
			CurrentFade = FinalFadeAmount;
			ShouldHide = false;
		}
		// Postavi vidljivost providnog materijala na trenutnu vidljivost
		ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("DitherOpacity"), CurrentFade);


	}
	// Ako objekat treba da bude vidljiv, uvecaj mu vidljivost
	else if (ShouldShow) {
		CurrentFade += DeltaTime * FadeRate;


		// Ako je trenutna vidljivost veca od maksimalne, odseci je i reci objektu da se ne prikazuje vise
		// Promeni materijal na solidni kad dodjes do maksimalne vidljivosti
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