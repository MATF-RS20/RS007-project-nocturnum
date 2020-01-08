#include "NocturnumEnemyController.h"
#include "NocturnumEnemyBot.h"
#include "NocturnumCharacter.h"
#include "AIWaypoint.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANocturnumEnemyController::ANocturnumEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ANocturnumEnemyController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void ANocturnumEnemyController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("All Systems Set"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Some Problem Occured"));
	}

}

void ANocturnumEnemyController::Possess(APawn * Pawn)
{
	Super::Possess(Pawn);
}

void ANocturnumEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TArray<AActor*> Players;
	TArray<AActor*> Bots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANocturnumEnemyBot::StaticClass(), Bots);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANocturnumCharacter::StaticClass(), Players);

	if (Bots.Num() > 0 && Players.Num() > 0) {
		ANocturnumEnemyBot* Bot = Cast<ANocturnumEnemyBot >(Bots[0]);
		ANocturnumCharacter* Player = Cast<ANocturnumCharacter >(Players[0]);

		PlayerDistance = Bot->GetDistanceTo(Player);

		if (PlayerDistance > AISightRadius) {
			bIsPlayerDetected = false;
		}
		else if (PlayerDistance < AISightRadius * 0.5f) {
			bIsPlayerDetected = true;
		}

		if (Bot != nullptr) {
			if (bIsPlayerDetected == true) {
				MoveToActor(Player, 5.0f);
			}
			else if (Bot->NextWaypoint != nullptr) {
				MoveToActor(Bot->NextWaypoint, 5.0f);
			}
		}
	}
}

FRotator ANocturnumEnemyController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}

	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);

}

void ANocturnumEnemyController::OnPawnDetected(TArray<AActor*> DetectedPawns)
{
	bIsPlayerDetected = true;
}