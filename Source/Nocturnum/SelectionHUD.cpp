// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionHUD.h"
#include "Engine.h"

void ASelectionHUD::DrawHUD() {

	if (bStartSelecting == true) {
		if (SelectedActors.Num() > 0)
		{
			for (int32 i = 0; i < SelectedActors.Num(); i++)
			{
				SelectedActors[i]->SetDeselected();
			}
		}
		SelectedActors.Empty();
		CurrentPoint = GetMousePosition();
		DrawRect(FLinearColor(0, 0, 1, 0.15f),
			InitialPoint.X, InitialPoint.Y,
			CurrentPoint.X - InitialPoint.X,
			CurrentPoint.Y - InitialPoint.Y);
		GetActorsInSelectionRectangle<ANocturnumCharacter>(InitialPoint, CurrentPoint, SelectedActors, false, false);

		if (SelectedActors.Num() > 0)
		{
			for (int32 i = 0; i < SelectedActors.Num(); i++)
			{
				SelectedActors[i]->SetSelected();
			}
		}
	}
}
/*
if (GEngine) {
	FString TheFloatStr = FString::SanitizeFloat(SpringArm->GetComponentRotation().Yaw);
	GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Yellow, *TheFloatStr);
}
*/


FVector2D ASelectionHUD::GetMousePosition() {
	float PointX;
	float PointY;
	GetOwningPlayerController()->GetMousePosition(PointX, PointY);

	return FVector2D(PointX, PointY);
}
