// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h" 
#include "Materials/MaterialInstanceDynamic.h" 
#include "Kismet/KismetMaterialLibrary.h"	
#include "Classes/Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h" 
#include "HidableObject.generated.h"

UCLASS()
class NOCTURNUM_API AHidableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHidableObject();

	UPROPERTY(BlueprintReadOnly, Category = Display)
		bool ShouldHide;
	UPROPERTY(BlueprintReadOnly, Category = Display)
		bool ShouldShow;
	UPROPERTY(EditAnywhere, Category = Display)
		float FinalFadeAmount;
	UPROPERTY(EditAnywhere, Category = Display)
		float CurrentFade;
	UPROPERTY(EditAnywhere, Category = Display)
		float FadeRate;
	UPROPERTY(EditAnywhere, Category = Material)
		UMaterial* OpaqueMaterial;
	UPROPERTY(EditAnywhere, Category = Material)
		UMaterial* TranslucentMaterial;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HideObject();
	virtual void ShowObject();

private:
	UPROPERTY(EditAnywhere, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ObjectMeshComponent;

	virtual void ChangeMeshMaterialToOpaque();
	virtual void ChangeMeshMaterialToTranslucent();

};
