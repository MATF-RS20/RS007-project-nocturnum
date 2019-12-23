// Fill out your copyright notice in the Description page of Project Settings.

#include "HidableObject.h"


// Sets default values
AHidableObject::AHidableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall Mesh Component"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'"));
	UStaticMesh* Asset = MeshAsset.Object;
	ObjectMeshComponent->SetStaticMesh(Asset);


	ShouldHide = false;
	ShouldShow = false;
	FinalFadeAmount = 0.4f; // Minimum Opacity
	CurrentFade = 1.0f;		// Maximum Opacity by default
	FadeRate = 2.0f;		// Rate at which to fade between max and min opacity
	
	 // Load our translucent material 
	static ConstructorHelpers::FObjectFinder<UMaterial> t_Mat(TEXT("Material'/Game/Materials/MTranslucentDefault.MTranslucentDefault'"));
	if (t_Mat.Object != NULL) {
		TranslucentMaterial = t_Mat.Object;
	}
	// Load our opaque material
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
		//ObjectMeshComponent->CreateAndSetMaterialInstanceDynamic(0)->SetScalarParameterValue(TEXT("Opacity"), CurrentFade);
		ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("Opacity"), CurrentFade);
		

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
		// Set the translucent material's Opacity property to CurrentFade
		//ObjectMeshComponent->CreateAndSetMaterialInstanceDynamic(0)->SetScalarParameterValue(TEXT("Opacity"), CurrentFade);
		ObjectMeshComponent->SetScalarParameterValueOnMaterials(TEXT("Opacity"), CurrentFade);
	}
}

void AHidableObject::HideObject() {
	if (!ShouldHide) {
		ShouldShow = false;
		ShouldHide = true;
		ChangeMeshMaterialToTranslucent();
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

void AHidableObject::ChangeMeshMaterialToTranslucent() {
	if (TranslucentMaterial){
		ObjectMeshComponent->SetMaterial(0, TranslucentMaterial);
	}
}

