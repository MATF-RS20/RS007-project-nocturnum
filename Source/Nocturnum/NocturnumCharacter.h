// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HidableObject.h"
#include "GameFramework/Character.h"
#include "NocturnumCharacter.generated.h"

UCLASS(Blueprintable)
class ANocturnumCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANocturnumCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetSelected();
	void SetDeselected();

	UFUNCTION(BlueprintPure, Category = "Health")
		float GetMaxHealth();
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetCurrentHealth();


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	// A selection decal for our character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UDecalComponent* SelectionDecal;

	
	TArray<AHidableObject*> BlockingObjects;
	virtual void DetectBlockingObjects();

	// Health parameters
	UPROPERTY(EditAnywhere, Category = "Health")
		float MaxHealth;
	UPROPERTY(EditAnywhere, Category = "Health")
		float CurrentHealth;
	UPROPERTY(EditAnywhere, Category = "Health")
		float HealthRegenRate;

	UFUNCTION()
		void AddHitsToObjectList(const TArray<FHitResult> &HitList, TArray<AHidableObject*>& Objects);

};

