// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionHUD.h"
#include "Engine.h"

void ASelectionHUD::DrawHUD() {

	DrawHealthBars();

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


void ASelectionHUD::DrawHealthBars() const
{
	//TSubclassOf<ANocturnumCharacter> classToFind;
	//classToFind = ANocturnumCharacter::StaticClass();
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, AllActors);

	//Prodjemo kroz sve pawnove
	for (FConstPawnIterator Pawn = GetWorld()->GetPawnIterator(); Pawn; ++Pawn) {

		//Ako je pawn nas karakter i ako je ziv
		ANocturnumCharacter* NocChar = Cast<ANocturnumCharacter>(*Pawn);
		if (NocChar && NocChar->GetCurrentHealth() > 0) {

			FVector Center = NocChar->GetActorLocation();
			//Zelimo da healthbar bude malo iznad karaktera
			FVector Offset = FVector(0.f, 0.f, 250.0f);

			//Transliranje koordinata iz sveta na ekran
			//Mnogo lakse ovde neko u projektu iz RG-a :s
			FVector2D ScreenPosition = FVector2D(Canvas->Project(Center + Offset));

			float HealthPercentage = NocChar->GetCurrentHealth() / NocChar->GetMaxHealth();
			//Dimenzije health bar-a

			float ActorOffset = NocChar->GetCapsuleComponent()->GetScaledCapsuleRadius();
			FVector BarA = Canvas->Project(FVector(Center.X, Center.Y - ActorOffset * 2, Center.Z + Offset.Z));
			FVector BarB = Canvas->Project(FVector(Center.X, Center.Y + ActorOffset * 2, Center.Z + Offset.Z));

			float BarWidth = (BarB - BarA).Size2D();
			float BarHeight = BarWidth * 0.2f;

			float X = ScreenPosition.X - BarWidth * 0.5f;
			float Y = ScreenPosition.Y;

			FCanvasTileItem TileItem(FVector2D(X, Y), FVector2D(BarWidth, BarHeight), FLinearColor(0.0f, 0.0f, 0.0f, 0.5f));
			TileItem.BlendMode = SE_BLEND_Translucent;
			Canvas->DrawItem(TileItem);

			BarWidth -= 3.f;
			BarHeight -= 3.f;

			X = ScreenPosition.X - BarWidth * 0.5f;
			Y = ScreenPosition.Y + 1.f;

			TileItem.Position = FVector2D(X, Y);
			TileItem.SetColor(FLinearColor(0.57f, 0.92f, 0.2f, 1.f));
			TileItem.Size = FVector2D(BarWidth * HealthPercentage, BarHeight);
			Canvas->DrawItem(TileItem);
		}
	}
}

FVector2D ASelectionHUD::GetMousePosition() {
	float PointX;
	float PointY;
	GetOwningPlayerController()->GetMousePosition(PointX, PointY);

	return FVector2D(PointX, PointY);
}
