// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIWaypoint.h"
#include "NocturnumEnemyBot.h"
#include "GameFramework/Actor.h"
#include "GeneralizedRoom.h"
#include "Kismet/GameplayStatics.h"
#include "DungeonGenerator.generated.h"

enum DoorwayStatus { BLOCKED, OPEN, REQUIRED };

enum ObjectiveType { START, END, OTHER };

struct RoomCell {

	RoomCell() {}

	//  +Y
	DoorwayStatus north = OPEN;
	// -Y
	DoorwayStatus south = OPEN;
	// -X
	DoorwayStatus east = OPEN;
	// +X
	DoorwayStatus west = OPEN;

	// da li je generator inicijalizovao sobu
	bool initialized = false;
	// da li je celije ozacena kao potrebna
	bool requiredPath = false;
	// ako je celija u putu koji mora da se nastavi, koji je njen indeks
	int32 pathKey = 0;


	ARoom* cell_room; // referenca na prostoriju u ovoj celiji

};

struct RoomRow {
	TArray<RoomCell> roomRow;
};

UCLASS()
class NOCTURNUM_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

private:
	
	TArray<RoomRow> all_rooms;
	int32 cells_x;
	int32 cells_y;

	// kordinate od kojih pocinjemo
	int32 origin_x;
	int32 origin_y;

	// rastojanje iz centara celija
	float cell_length = 3000.0f;

	ARoom* entryCell = NULL;

	// broj trenutno generisanih celija
	int32 occupiedCells = 0;
	// prosecan broj ulaza
	float avgDegree = 0;
	// ukupan broj ulaza, stepen cvora
	float totalDegree = 0;
	// broj celija koji ce biti u sklopu putanje
	int32 minNumCells = 0;
	// najmanji broj celija koji ce nivo imati
	int32 projectedNumCells = 0;

	// broj puteva koje je generator pronasao
	int32 pathsFinished = 0;
	// broj puteva koje je pretrazio
	int32 totalPaths = 0;

	// da li je generator naleteo na vec napravljen put
	bool reachedSubsequentPath = false;

public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TArray< TSubclassOf<ARoom> > Room_1_Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TArray< TSubclassOf<ARoom> > Room_2_Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TArray< TSubclassOf<ARoom> > Room_3_Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TArray< TSubclassOf<ARoom> > Room_4_Doors;

	// Niz soba koje imaju neki cilj
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Modules")
		TArray<TSubclassOf<AGeneralizedRoom>> ObjectiveRooms;

	// gustina grananja nivoa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float targetDensity = 2.35;

	// sansa da ce se stepen promeniti kako bi ispratio gustinu
	// treba da bude izmedju 0 i 1, ako je >=1 onda ce se uvek menjati ako je <= 0 onda se nikada nece menjati.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float conformity = 0.5;

	// maksimalni odnos (svih celija) / (potrebnih celija)
	// ako taj broj premasuje softLimitCellRatio * minNumCells,
	// onda umanji stepene preostalih cvorova
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float softLimitCellRatio = 2.3;

	// lista objektiva na nivou
	TArray<AGeneralizedRoom*> allObjectives;
	TArray<TPair<int32, int32>> allObjEnter;
	TArray<TPair<int32, int32>> allObjExit;
	int currentObjectiveIndex = 0;

	// sledeci objektiv koji igrac mora da ispuni
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modules")
		AGeneralizedRoom* CurrentObjectiveRoom;

	// pocetak nivoa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TSubclassOf < AGeneralizedRoom> StartRoom;

	// kraj nivoa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modules")
		TSubclassOf < AGeneralizedRoom> EndRoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TSubclassOf < ARoom> testMarker;

	// Generisi ceo nivo
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		void GenerateDungeon(int32 x, int32 y, int32 start_x, int32 start_y);

	// Inicijalizuj Waypoint-ove
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		void InitializeWaypoints();

	// Inicijalizuj pocetne Waypoint-ove za botove
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		void InitializeBotStartingWaypoints(TArray<AActor*> ActiveWaypoints, int32 NumOfWaypoints);

	bool bigRoomFits(int32 x, int32 y,
		TArray<FVector2D>& allCells,
		FVector2D& enter, FVector2D& exit,
		FVector2D& entryDir, FVector2D& exitDir);

	// stavi GeneralizedRoom na scenu i vrati referencu ako je uspela u suprotnom NULL.
	AGeneralizedRoom* GenerateBigRoom(int32 x, int32 y,
		int32& xEnter, int32& yEnter,
		int32& xExit, int32& yExit,
		ObjectiveType type = OTHER);

	// nalazi putanju izmedju celija za rekurzivnu generaciju
	void GeneratePath(int32 x0, int32 y0, int32 x1, int32 y1);

	// rekurzivna generacija celija
	void GenerateCell(int32 x, int32 y);

	AActor* playerReference = NULL;
	int playerX;
	int playerY;
	int prevPlayerX;
	int prevPlayerY;

	//X i Y koordinata u zavisnosti na igraca
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		void CalcPlayerCell();

	// koordinate celije u kojoj se nalazi igrac
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		FVector2D GetPlayerCellLocation();

	// mapira lokaciju iz sveta u celiju
	UFUNCTION(BlueprintCallable, Category = "DungeonGenerator")
		FVector2D WorldToCell(FVector location);
	
	
};
