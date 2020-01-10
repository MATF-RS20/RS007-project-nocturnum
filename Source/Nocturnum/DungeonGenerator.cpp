// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator.h"


// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	int eastWestCells = FMath::RandRange(6, 9);
	int northSouthCells = FMath::RandRange(6, 9);
	int startX = FMath::RandRange(1, 3);
	int startY = FMath::RandRange(1, 3);

	GenerateDungeon(eastWestCells, northSouthCells, startX, startY);
	InitializeWaypoints();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ADungeonGenerator::GenerateDungeon(int32 x, int32 y, int32 start_x, int32 start_y)
{
	cells_x = x;
	cells_y = y;

	// napuni sa praznim redovima
	for (int i = 0; i < x; i++) {
		RoomRow newRow = RoomRow();
		for (int j = 0; j < y; j++) {
			RoomCell newRoom = RoomCell();
			newRow.roomRow.Add(newRoom);
		}
		all_rooms.Add(newRow);
	}

	// zapocni generaciju dungeona
	origin_x = start_x;
	origin_y = start_y;

	allObjectives = TArray<AGeneralizedRoom*>();

	// generisi pocetnu sobu
	int32 entryX, entryY, exitX, exitY;

	AGeneralizedRoom* entrance = GenerateBigRoom(origin_x, origin_y,
		entryX, entryY, exitX, exitY, START);

	allObjectives.Add(entrance);
	TPair<int32, int32> p1 = TPair<int32, int32>();
	p1.Key = entryX;
	p1.Value = entryY;

	TPair<int32, int32> p2 = TPair<int32, int32>();
	p2.Key = exitX;
	p2.Value = exitY;
	allObjEnter.Add(TPair<int32, int32>(p1));
	allObjExit.Add(TPair<int32, int32>(p2));


	// generisi do 3 nove sobe
	for (int i = 0; i < 3; i++) {
		int32 objX, objY, objX1, objY1;
		AGeneralizedRoom* objective = GenerateBigRoom(origin_x + 2, origin_y + 2,
			objX, objY, objX1, objY1);

		if (objective) {
			p1.Key = objX;
			p1.Value = objY;
			p2.Key = objX1;
			p2.Value = objY1;

			allObjectives.Add(objective);
			allObjEnter.Add(TPair<int32, int32>(p1));
			allObjExit.Add(TPair<int32, int32>(p2));
		}
	}



	// generisi validnu krajnu sobu
	AGeneralizedRoom* endingRoom = NULL;
	int32 endX, endY, endX1, endY1;
	while (endingRoom == NULL) {
		endingRoom = GenerateBigRoom(origin_x + 2, origin_y + 2,
			endX, endY, endX1, endY1, END);
	}

	allObjectives.Add(endingRoom);
	p1.Key = endX;
	p1.Value = endY;
	p2.Key = endX1;
	p2.Value = endY1;
	allObjEnter.Add(TPair<int32, int32>(p1));
	allObjExit.Add(TPair<int32, int32>(p2));


	// generisi sve putanje
	for (int i = 0; i < allObjEnter.Num() - 1; i++) {
		GeneratePath(allObjExit[i].Key, allObjExit[i].Value,
			allObjEnter[i + 1].Key, allObjEnter[i + 1].Value);
		totalPaths++;
	}

	// rekurzivna generacija u svim cvorovima
	for (int i = 0; i < allObjEnter.Num(); i++) {
		reachedSubsequentPath = false;
		GenerateCell(allObjEnter[i].Key, allObjEnter[i].Value);
		GenerateCell(allObjExit[i].Key, allObjExit[i].Value);
		pathsFinished += pathsFinished >= totalPaths ? 0 : 1;

	}

	// sve je izgenerisano postavi objective na pocetni i zapocni igru
	currentObjectiveIndex = 0;
	CurrentObjectiveRoom = allObjectives[currentObjectiveIndex];
	CurrentObjectiveRoom->isAccessible = true;

}

void ADungeonGenerator::InitializeWaypoints()
{
	// Ucitaj sve Waypointove u niz
	TArray<AActor*> ActiveWaypoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIWaypoint::StaticClass(), ActiveWaypoints);

	int32 NumOfWaypoints = ActiveWaypoints.Num();
	UE_LOG(LogTemp, Warning, TEXT("Number of Waypoints detected: %d"), NumOfWaypoints);

	// Ako postoje bar 2 Waypointa, prodji kroz sve i napravi ciklicnu listu od njih
	if (NumOfWaypoints > 1) {
		AAIWaypoint* PreviousWaypoint = Cast<AAIWaypoint>(ActiveWaypoints[0]);
		AAIWaypoint* NextWaypoint = nullptr;

		for (int32 i = 1; i != NumOfWaypoints; i++) {
			NextWaypoint = Cast<AAIWaypoint>(ActiveWaypoints[i]);
			PreviousWaypoint->NextWaypoint = NextWaypoint;
			PreviousWaypoint = NextWaypoint;

			// Postavljamo da poslednji pokazuje na prvi
			if (i == NumOfWaypoints - 1) {
				NextWaypoint->NextWaypoint = Cast<AAIWaypoint>(ActiveWaypoints[0]);
			}
		}
		// Postavljamo pocetni Waypoint za svakog lika
		InitializeBotStartingWaypoints(ActiveWaypoints, NumOfWaypoints);
	}
}

void ADungeonGenerator::InitializeBotStartingWaypoints(TArray<AActor*> ActiveWaypoints, int32 NumOfWaypoints)
{
	// Ucitaj sve nase likove u niz
	TArray<AActor*> EnemyBots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANocturnumEnemyBot::StaticClass(), EnemyBots);
	int32 NumOfEnemyBots = EnemyBots.Num();

	UE_LOG(LogTemp, Warning, TEXT("Number of Bots detected: %d"), NumOfEnemyBots);

	if (NumOfEnemyBots > 0) {
		for (int32 i = 0; i != NumOfEnemyBots; i++) {
			ANocturnumEnemyBot* Bot = Cast<ANocturnumEnemyBot>(EnemyBots[i]);
			if (Bot != nullptr) {
				// Postavi nasumicni Waypoint kao pocetni
				Bot->NextWaypoint = Cast<AAIWaypoint>(ActiveWaypoints[FMath::RandRange(0, NumOfWaypoints - 1)]);
				UE_LOG(LogTemp, Warning, TEXT("Starting Waypint for Bot %d is: %s"), NumOfEnemyBots, *Bot->NextWaypoint->GetActorLocation().ToString());
			}
		}
	}
}

bool ADungeonGenerator::bigRoomFits(int32 x, int32 y,
	TArray<FVector2D>& allCells,
	FVector2D& enter, FVector2D& exit,
	FVector2D& entryDir, FVector2D& exitDir)
{
	// proveravamo da li je van granica
	// proveravamo da li je na tom mestu vec nesto inicijalizovano
	// proveravamo da li susedne prostorije zahtevaju vrata
	for (FVector2D room : allCells) {
		int testX = FPlatformMath::RoundToInt(room.X) + x;
		int testY = FPlatformMath::RoundToInt(room.Y) + y;
		if (testX < 0 || testX >= cells_x) 
			return false; // van granica
		if (testY < 0 || testY >= cells_y) 
			return false; // van granica
		if (all_rooms[testX].roomRow[testY].initialized) 
			return false; // vec inicijalizovano mesto

		bool isDoorway = room.Equals(enter);
		//da li neko zahteva vrata
		if (testX + 1 < cells_x) {
			if (all_rooms[testX + 1].roomRow[testY].east == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX + 1, testY).Equals(FVector2D(testX, testY) + entryDir))
						return false;
				}
				else
					return false;
			}
		}

		if (testX - 1 >= 0) {
			if (all_rooms[testX - 1].roomRow[testY].west == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX - 1, testY).Equals(FVector2D(testX, testY) + entryDir)) 
						return false;
				}
				else 
					return false;
			}
		}

		if (testY + 1 < cells_y) {
			if (all_rooms[testX].roomRow[testY + 1].south == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX, testY + 1).Equals(FVector2D(testX, testY) + entryDir)) 
						return false;
				}
				else 
					return false;
			}
		}

		if (testY - 1 >= 0) {
			if (all_rooms[testX].roomRow[testY - 1].north == REQUIRED) {
				if (isDoorway) {
					if (!FVector2D(testX, testY - 1).Equals(FVector2D(testX, testY) + entryDir)) 
						return false;
				}
				else 
					return false;
			}
		}

		for (int i = 0; i < allObjEnter.Num(); i++) {
			TPair<int, int> a = allObjEnter[i];
			TPair<int, int> b = allObjExit[i];

			if (testX == a.Key || testX == b.Key) 
				return false;
			if (testY == a.Value || testY == b.Value) 
				return false;
		}


	}
	// proveri da li ulaz vodi do blokiranih delova ili je van granica
	int e1x = FPlatformMath::RoundToInt(enter.X) + FPlatformMath::RoundToInt(entryDir.X) + x;
	int e1y = FPlatformMath::RoundToInt(enter.Y) + FPlatformMath::RoundToInt(entryDir.Y) + y;
	if (e1x < 0 || e1x >= cells_x) 
		return false;
	if (e1y < 0 || e1y >= cells_y) 
		return false;
	if (entryDir.Equals(FVector2D(1, 0))) {
		if (all_rooms[e1x].roomRow[e1y].east == BLOCKED) 
			return false;
	}
	else if (entryDir.Equals(FVector2D(-1, 0))) {
		if (all_rooms[e1x].roomRow[e1y].west == BLOCKED) 
			return false;
	}
	else if (entryDir.Equals(FVector2D(0, 1))) {
		if (all_rooms[e1x].roomRow[e1y].south == BLOCKED) 
			return false;
	}
	else if (entryDir.Equals(FVector2D(0, -1))) {
		if (all_rooms[e1x].roomRow[e1y].north == BLOCKED) 
			return false;
	}

	int e2x = FPlatformMath::RoundToInt(exit.X) + FPlatformMath::RoundToInt(exitDir.X) + x;
	int e2y = FPlatformMath::RoundToInt(exit.Y) + FPlatformMath::RoundToInt(exitDir.Y) + y;
	if (e2x < 0 || e2x >= cells_x) 
		return false;
	if (e2y < 0 || e2y >= cells_y) 
		return false;
	if (exitDir.Equals(FVector2D(1, 0))) {
		if (all_rooms[e2x].roomRow[e2y].east == BLOCKED) 
			return false;
	}
	else if (exitDir.Equals(FVector2D(-1, 0))) {
		if (all_rooms[e2x].roomRow[e2y].west == BLOCKED) 
			return false;
	}
	else if (exitDir.Equals(FVector2D(0, 1))) {
		if (all_rooms[e2x].roomRow[e2y].south == BLOCKED) 
			return false;
	}
	else if (exitDir.Equals(FVector2D(0, -1))) {
		if (all_rooms[e2x].roomRow[e2y].north == BLOCKED) 
			return false;
	}
	// provera je prosla, prostorija moze da se smesti
	return true;
}

void clockwiseVector(FVector2D& vec)
{
	int temp = vec.X;
	vec.X = -vec.Y;
	vec.Y = temp;
}


AGeneralizedRoom* ADungeonGenerator::GenerateBigRoom(int32 x, int32 y,
	int32& xEnter, int32& yEnter, int32& xExit, int32& yExit,
	ObjectiveType type)
{
	xEnter = -1;
	yEnter = -1;
	xExit = -1;
	yExit = -1;

	AGeneralizedRoom* room;
	if (type == START) {
		room = StartRoom->GetDefaultObject<AGeneralizedRoom>();
	}
	else if (type == END) {
		room = EndRoom->GetDefaultObject<AGeneralizedRoom>();
	}
	else {
		if (ObjectiveRooms.Num() == 0) {
			room = StartRoom->GetDefaultObject<AGeneralizedRoom>();
		}
		else {
			room = ObjectiveRooms[FMath::RandRange(0,
				ObjectiveRooms.Num() - 1)]->GetDefaultObject<AGeneralizedRoom>();
		}

	}

	AGeneralizedRoom* spawned_room = NULL;

	// kopiraj sva podesavanja za postavljanje
	TArray<FVector2D> roomspace = TArray<FVector2D>(room->occupiedCells);
	FVector2D enter = FVector2D(room->entranceCell);
	FVector2D exit = FVector2D(room->exitCell);
	FVector2D enterDir = FVector2D(room->entranceDirection);
	FVector2D exitDir = FVector2D(room->exitDirection);

	// pokusaj da postavis u blizini ove lokacije
	for (int tries = 8; tries > 0; tries--) {
		int x1 = x + FMath::RandRange(-2, 2);
		FMath::Clamp(x1, 0, cells_x - 1);
		int y1 = y + FMath::RandRange(-2, 2);
		FMath::Clamp(y1, 0, cells_y - 1);

		if (type == START) 
			x1 = x;
		if (type == START) 
			y1 = y;

		int startRotations = 0;

		// pokusaj sve rotacije na ovoj lokaciji
		for (int rotations = 0; rotations < 4; rotations++) {

			bool fit = bigRoomFits(x1, y1,
				roomspace, enter, exit, enterDir, exitDir);

			if (fit) {
				// pronadjen nacin da prostorija stane

				// primeni dosadasnje transformacije
				FVector location = FVector((x1 - origin_x) * cell_length,
					(y1 - origin_y) * cell_length, 0);
				FRotator rotation = FRotator(0, (rotations + startRotations)* 90.0f, 0);
				FTransform transform = FTransform(rotation, location);

				// napravi sobu na toj poziciji
				UWorld* const World = GetWorld();
				if (World) {
					if (room) {
						spawned_room =
							World->SpawnActor<AGeneralizedRoom>(room->GetClass(), transform);
					}
					else { UE_LOG(LogTemp, Warning, TEXT("Soba je nullptr")); }
				}
				else { UE_LOG(LogTemp, Warning, TEXT("Svet je nullptr")); }

				// oznaci i pripremi celije
				for (int i = 0; i < roomspace.Num(); i++) {
					FVector2D cell = roomspace[i];
					int currentX = FGenericPlatformMath::RoundToInt(cell.X) + x1;
					int currentY = FGenericPlatformMath::RoundToInt(cell.Y) + y1;

					all_rooms[currentX].roomRow[currentY].cell_room = spawned_room;
					all_rooms[currentX].roomRow[currentY].initialized = true;

					all_rooms[currentX].roomRow[currentY].north = BLOCKED;
					all_rooms[currentX].roomRow[currentY].east = BLOCKED;
					all_rooms[currentX].roomRow[currentY].south = BLOCKED;
					all_rooms[currentX].roomRow[currentY].west = BLOCKED;

					if (cell.Equals(enter)) {
						if (enterDir.Equals(FVector2D(1, 0))) {
							all_rooms[currentX].roomRow[currentY].west = REQUIRED;
						}
						else if (enterDir.Equals(FVector2D(0, 1))) {
							all_rooms[currentX].roomRow[currentY].north = REQUIRED;
						}
						else if (enterDir.Equals(FVector2D(-1, 0))) {
							all_rooms[currentX].roomRow[currentY].east = REQUIRED;
						}
						else {
							all_rooms[currentX].roomRow[currentY].south = REQUIRED;
						}

						xEnter = x1 + FGenericPlatformMath::RoundToInt(cell.X + enterDir.X);
						yEnter = y1 + FGenericPlatformMath::RoundToInt(cell.Y + enterDir.Y);
					}
					if (cell.Equals(exit)) {
						if (exitDir.Equals(FVector2D(1, 0))) {
							all_rooms[currentX].roomRow[currentY].west = REQUIRED;
						}
						else if (exitDir.Equals(FVector2D(0, 1))) {
							all_rooms[currentX].roomRow[currentY].north = REQUIRED;
						}
						else if (exitDir.Equals(FVector2D(-1, 0))) {
							all_rooms[currentX].roomRow[currentY].east = REQUIRED;
						}
						else {
							all_rooms[currentX].roomRow[currentY].south = REQUIRED;
						}
						xExit = x1 + FGenericPlatformMath::RoundToInt(cell.X + exitDir.X);
						yExit = y1 + FGenericPlatformMath::RoundToInt(cell.Y + exitDir.Y);
					}

				}
				// vrati prostoriju po referenci
				return spawned_room;
			}

			// ako smo dosli dovde znaci da nije uspelo, rotiraj i probaj opet.
			clockwiseVector(enter);
			clockwiseVector(exit);
			clockwiseVector(enterDir);
			clockwiseVector(exitDir);
			for (int i = 0; i < roomspace.Num(); i++) {
				FVector2D c = roomspace[i];
				clockwiseVector(c);
				roomspace[i] = c;
			}
		}
	}
	return NULL; // prostorija ne moze da se postavi na ovoj lokaciji
}


struct RowPath {
	TArray<TPair<int32, int32>> p; // Unreal mrzi matrice :(
};

struct Row2D {
	TArray<RowPath> row; // Bas, bas mrzi matrice..
};

bool PathConnect(TArray<RowPath>& pq, TArray<Row2D>& ctp,
	RowPath& p, int32 x, int32 y) {
	if (x < 0 || x >= ctp.Num()) 
		return false;
	if (y < 0 || y >= ctp[0].row.Num()) 
		return false;

	if (ctp[x].row[y].p.Num() == 0 ||
		p.p.Num() + 1 < ctp[x].row[y].p.Num()) {
		// ako ovde ne postoji put ili je dosadasnji put gori od ovog
		TArray<TPair<int32, int32>> newpath = TArray<TPair<int32, int32>>(p.p);
		TPair<int32, int32> newpt = TPair<int32, int32>();
		newpt.Key = x; newpt.Value = y;
		newpath.Push(newpt);
		RowPath np = RowPath();
		np.p = newpath;
		pq.Push(np);

		ctp[x].row[y].p = newpath;
		return true;
	}
	return false;
}

void ADungeonGenerator::GeneratePath(int32 x0, int32 y0, int32 x1, int32 y1)
{
	if (x1 == -1 || y1 == -1) {
		return;
	}

	// tabela putanja, iste velicine kao dungeon
	TArray<Row2D> cellsToPaths = TArray<Row2D>();
	// inicijalizacija praznih celija
	for (int i = 0; i < cells_x; i++) {
		Row2D row = Row2D();
		for (int j = 0; j < cells_y; j++) {
			RowPath p = RowPath();
			p.p = TArray<TPair<int32, int32>>();
			row.row.Add(p);
		}
		cellsToPaths.Add(row);
	}

	// napravi listu koja ce imati red putanje koja se pravi
	TArray<RowPath> pathQueue = TArray<RowPath>();

	TArray<TPair<int32, int32>> path = TArray<TPair<int32, int32>>();
	// pocinjemo od (x0, y0)
	TPair<int32, int32> origin = TPair<int32, int32>();
	origin.Key = x0;
	origin.Value = y0;
	path.Add(origin);

	cellsToPaths[x0].row[y0].p = path;
	RowPath rp = RowPath(); rp.p = path; pathQueue.Push(rp);
	// uzmi sledecu putanju iz reda
	while (pathQueue.Num() > 0) {
		RowPath current = pathQueue.Pop();
		TPair<int32, int32> currentEnd = current.p.Last();

		RoomCell room;

		// proveri celije u svim pravcima

		if (currentEnd.Key + 1 >= 0 && currentEnd.Key + 1 < all_rooms.Num()
			&& currentEnd.Value >= 0 && currentEnd.Value < all_rooms[0].roomRow.Num()) {

			room = all_rooms[currentEnd.Key + 1].roomRow[currentEnd.Value];
			// ako je moguce povezati onda povezi
			if (room.east != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key + 1, currentEnd.Value);
			}
		}

		if (currentEnd.Key - 1 >= 0 && currentEnd.Key - 1 < all_rooms.Num()
			&& currentEnd.Value >= 0 && currentEnd.Value < all_rooms[0].roomRow.Num()) {

			room = all_rooms[currentEnd.Key - 1].roomRow[currentEnd.Value];
			// ako je moguce povezati onda povezi
			if (room.west != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key - 1, currentEnd.Value);
			}
		}


		if (currentEnd.Key >= 0 && currentEnd.Key< all_rooms.Num()
			&& currentEnd.Value + 1 >= 0 && currentEnd.Value + 1 < all_rooms[0].roomRow.Num()) {

			room = all_rooms[currentEnd.Key].roomRow[currentEnd.Value + 1];
			// ako je moguce povezati onda povezi
			if (room.south != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key, currentEnd.Value + 1);
			}
		}

		if (currentEnd.Key >= 0 && currentEnd.Key< all_rooms.Num()
			&& currentEnd.Value - 1 >= 0 && currentEnd.Value - 1 < all_rooms[0].roomRow.Num()) {

			room = all_rooms[currentEnd.Key].roomRow[currentEnd.Value - 1];
			// ako je moguce povezati onda povezi
			if (room.north != BLOCKED && !room.initialized) {
				bool connect = PathConnect(pathQueue, cellsToPaths, current,
					currentEnd.Key, currentEnd.Value - 1);
			}

		}
	}

	// ako je putanja sece sa vec pronadjenom putanjom
	bool foundIntersection = false;

	// oznaci konacnu putanju i pronadjena vrata
	TArray<TPair<int32, int32>> finalPath = cellsToPaths[x1].row[y1].p;
	for (int i = 0; i < finalPath.Num(); i++) {
		TPair<int32, int32> curr = finalPath[i];
		RoomCell r = all_rooms[curr.Key].roomRow[curr.Value];

		if (i > 0) {
			TPair<int32, int32> prev = finalPath[i - 1];
			if (prev.Key + 1 == curr.Key) {
				r.east = REQUIRED;
			}
			else if (prev.Key - 1 == curr.Key) {
				r.west = REQUIRED;
			}
			else if (prev.Value + 1 == curr.Value) {
				r.south = REQUIRED;
			}
			else if (prev.Value - 1 == curr.Value) {
				r.north = REQUIRED;
			}
		}

		if (i < finalPath.Num() - 1) {
			TPair<int32, int32> next = finalPath[i + 1];
			if (next.Key == curr.Key + 1) {
				r.west = REQUIRED;
			}
			else if (next.Key == curr.Key - 1) {
				r.east = REQUIRED;
			}
			else if (next.Value == curr.Value - 1) {
				r.south = REQUIRED;
			}
			else if (next.Value == curr.Value + 1) {
				r.north = REQUIRED;
			}

		}


		if (r.north != REQUIRED) 
			r.north = OPEN;
		if (r.east != REQUIRED) 
			r.east = OPEN;
		if (r.south != REQUIRED) 
			r.south = OPEN;
		if (r.west != REQUIRED)
			r.west = OPEN;

		// generatoru reci da si pronasao put
		if (r.requiredPath) {
			if (!foundIntersection) {
				// ako je put presek onda ce biti jedan manje rekurzivni poziv u generatoru
				foundIntersection = true;
				//totalPaths--;
			}
		}
		else {
			// ova celija mora biti generisana
			minNumCells++;
		}
		r.requiredPath = true;
		r.pathKey = totalPaths;

		all_rooms[curr.Key].roomRow[curr.Value] = r;

		if (testMarker) {
			UWorld* const World = GetWorld();
			if (World) {
				FVector location = FVector((curr.Key - origin_x) * cell_length,
					(curr.Value - origin_y) * cell_length, 1100);
				FTransform t = FTransform(location);
				ARoom* m = World->SpawnActor<ARoom>(testMarker->GetDefaultObject()->GetClass(), t);
			}
		}


	}

}

void rotateCW(bool& n, bool& e, bool& s, bool& w)
{
	bool t = w;
	w = s;
	s = e;
	e = n;
	n = t;
}

bool roomFits(DoorwayStatus adj_n,
	DoorwayStatus adj_e,
	DoorwayStatus adj_s,
	DoorwayStatus adj_w,
	bool n, bool e, bool s, bool w)
{
	// vraca false jer pokusava da stavi vrata na lokaciju koja je blokirana
	if (n && adj_n == DoorwayStatus::BLOCKED) 
		return false;
	if (e && adj_e == DoorwayStatus::BLOCKED) 
		return false;
	if (s && adj_s == DoorwayStatus::BLOCKED) 
		return false;
	if (w && adj_w == DoorwayStatus::BLOCKED) 
		return false;

	// vraca false ako nije stavio vrata tamo gde je potrebno da se nalaze vrata
	if (!n && adj_n == DoorwayStatus::REQUIRED) 
		return false;
	if (!e && adj_e == DoorwayStatus::REQUIRED) 
		return false;
	if (!s && adj_s == DoorwayStatus::REQUIRED) 
		return false;
	if (!w && adj_w == DoorwayStatus::REQUIRED) 
		return false;

	// sve je kako treba vracamo true
	return true;
}

void ADungeonGenerator::GenerateCell(int32 x, int32 y) {
	// ako je izasao van okvira nivoa vrati se
	if (x >= all_rooms.Num()) 
		return;
	if (y >= all_rooms[0].roomRow.Num()) 
		return;
	if (x < 0 || y < 0) 
		return;

	RoomCell currentCell = all_rooms[x].roomRow[y];

	// ako je ova prostorija vec generisana vrati se
	if (currentCell.initialized) 
		return;

	/*ako je generator naleteo na jos neku putanju, onda ce dovrsiti tu putanju, zbog toga
	ako naletimo na putanje koje se seku mozemo jednu putanju oznaciti kao dovrsenu*/
	if (currentCell.requiredPath && currentCell.pathKey > pathsFinished &&
		!reachedSubsequentPath) {
		reachedSubsequentPath = true;
		pathsFinished += pathsFinished >= totalPaths ? 0 : 1;
	}

	// status svake susedne celije : BLOCKED (ne moze da se poveveze), REQUIRED (mora tu da se nalazi) OPEN (moze tu da se nalazi)
	DoorwayStatus adj_w, adj_e, adj_s, adj_n;

	// vidi da li si u blizini granica, u suprotnom pogledaj u susedne celije
	if (x <= 0) 
		adj_e = DoorwayStatus::BLOCKED;
	else 
		adj_e = all_rooms[x - 1].roomRow[y].west;

	if (x >= cells_x - 1) 
		adj_w = DoorwayStatus::BLOCKED;
	else 
		adj_w = all_rooms[x + 1].roomRow[y].east;

	if (y <= 0) 
		adj_s = DoorwayStatus::BLOCKED;
	else 
		adj_s = all_rooms[x].roomRow[y - 1].north;

	if (y >= cells_y - 1) 
		adj_n = DoorwayStatus::BLOCKED;
	else 
		adj_n = all_rooms[x].roomRow[y + 1].south;

	// odredi koliko ce/moze sledeca soba imati ulaza
	int min = 0;
	if (adj_s == DoorwayStatus::REQUIRED) 
		min++;
	if (adj_n == DoorwayStatus::REQUIRED) 
		min++;
	if (adj_e == DoorwayStatus::REQUIRED) 
		min++;
	if (adj_w == DoorwayStatus::REQUIRED) 
		min++;
	int max = 4;
	if (adj_s == DoorwayStatus::BLOCKED) 
		max--;
	if (adj_n == DoorwayStatus::BLOCKED) 
		max--;
	if (adj_e == DoorwayStatus::BLOCKED) 
		max--;
	if (adj_w == DoorwayStatus::BLOCKED) 
		max--;

	// izaberi nasumican broj vrata na osnovu gustine
	if (min == 0) min++;
	int num_doorways = FMath::RandRange(min, max);

	float tempAvg = (num_doorways + totalDegree) / (occupiedCells + 1);
	float deltaAvg = tempAvg - avgDegree;
	// ako trenutni stepen cvora smanjuje gustinu, a pritom je gustina vec mala
	if (tempAvg < targetDensity) {
		// damo mu sansu da poveca stepen cvora na max
		if (FMath::FRandRange(0.0f, 0.99f) < conformity) {
			num_doorways = max;
		}
	}
	else if (tempAvg > targetDensity) {
		//u suprotnom ako povecavamo, a gustina je vec prevelika
		//dajemo mu sansu da stepen cvora bude minimum
		if (FMath::FRandRange(0.0f, 0.99f) < conformity) {
			num_doorways = min;
		}
	}

	// snapuemo na min da ispostujemo soft limit ako je narusen
	if ((float)projectedNumCells >= softLimitCellRatio * minNumCells *
		(pathsFinished >= totalPaths ? (1) : (pathsFinished + 1) / totalPaths)) {
		num_doorways = min;
	}

	if (occupiedCells == 0) {
		num_doorways = FGenericPlatformMath::RoundToInt(targetDensity);
		num_doorways = FMath::Clamp(num_doorways, min, max);
	}

	// izaberi nasumicnu sobu sa ovim brojem vrata
	int num_room_of_type;
	if (num_doorways == 1) 
		num_room_of_type = Room_1_Door.Num();
	else if (num_doorways == 2) 
		num_room_of_type = Room_2_Doors.Num();
	else if (num_doorways == 3) 
		num_room_of_type = Room_3_Doors.Num();
	else 
		num_room_of_type = Room_4_Doors.Num();

	// naoravi niz mogucih indeksa za sobe ove velicine
	TArray<int> index_selector;
	for (int i = 0; i < num_room_of_type; i++) 
		index_selector.Add(i);

	// bice popunjeni sa ulazima kandidata soba
	bool n = false;
	bool e = false;
	bool s = false;
	bool w = false;

	// da le je generisanje uspelo
	bool success = false;
	// broj pokusaja za ovu celiju, u suprotnom odustajemo
	int remainingTries = 5;

	while (index_selector.Num() > 0 && remainingTries > 0 && !success) {
		remainingTries--;

		// izaberi nasumicnu sobu koja do sad nije ispobana
		int selected = FMath::RandRange(0, index_selector.Num() - 1);
		selected = index_selector[selected];
		ARoom* room;
		if (num_doorways == 1)  
			room = Room_1_Door[selected]->GetDefaultObject<ARoom>();
		else if (num_doorways == 2) 
			room = Room_2_Doors[selected]->GetDefaultObject<ARoom>();
		else if (num_doorways == 3) 
			room = Room_3_Doors[selected]->GetDefaultObject<ARoom>();
		else 
			room = Room_4_Doors[selected]->GetDefaultObject<ARoom>();

		// popuni podatke o susedstvu (ulazima)
		n = room->north;
		e = room->east;
		s = room->south;
		w = room->west;

		// zapocni sa nasumicnom rotacijom
		int initialRotations = FMath::RandRange(0, 3);
		for (int i = 0; i < initialRotations; i++) {
			rotateCW(n, e, s, w);
		}

		// posto su vrata samo na cetiri strane sveta dovoljno je da probamo 4 puta
		for (int rotations = 0; rotations < 4; rotations++) {

			// provera da li moze da stane
			success = roomFits(adj_n, adj_e, adj_s, adj_w, n, e, s, w);

			if (success) {
				// uspesno smestena potrebno da primenimo transformacije potrebne za smestanje
				FVector location = FVector((x - origin_x) * cell_length,
					(y - origin_y) * cell_length, 0);
				FRotator rotation = FRotator(0, (rotations + initialRotations)* 90.0f, 0);
				FTransform transform = FTransform(rotation, location);

				currentCell.initialized = true;

				// spawnuj sobi na datoj poziciji
				UWorld* const World = GetWorld();
				if (World) {
					if (room) {
						ARoom* spawned_room = World->SpawnActor<ARoom>(room->GetClass(), transform);
						currentCell.cell_room = spawned_room;
					}
				}

				// oznaci ulaze ove celije za sledecu celiju
				if (n) 
					currentCell.north = DoorwayStatus::REQUIRED;
				else 
					currentCell.north = DoorwayStatus::BLOCKED;
				if (e) 
					currentCell.east = DoorwayStatus::REQUIRED;
				else 
					currentCell.east = DoorwayStatus::BLOCKED;
				if (s) 
					currentCell.south = DoorwayStatus::REQUIRED;
				else 
					currentCell.south = DoorwayStatus::BLOCKED;
				if (w) 
					currentCell.west = DoorwayStatus::REQUIRED;
				else 
					currentCell.west = DoorwayStatus::BLOCKED;

				all_rooms[x].roomRow[y] = currentCell;

				// dodaj na trenutnu kolicinu zauzetih celija
				occupiedCells++;
				totalDegree += num_doorways;
				avgDegree = totalDegree / occupiedCells;

				// dodaj kolko ce jos celija morati da se izgeneise zbog dodatnih ulaza
				projectedNumCells += num_doorways - min;

				break;
			}
			// nije uspeo da smesti sledeca rotacija
			rotateCW(n, e, s, w);
		}
		// nismo uspeli da smestimo prostoriju, izbaci je iz liste mogucih prostorija i nastavi dalje
		index_selector.Remove(selected);
	}

	// generisi novu celiju za svaka postavljena vrata ako je soba uspesno postavljena
	if (success) {

		// dodaj varijaciju na pravac rekurzije
		float r = FMath::FRandRange(0.0f, 1.0f);
		if (r < 0.25f) {
			if (n) 
				GenerateCell(x, y + 1);
			if (e) 
				GenerateCell(x - 1, y);
			if (s) 
				GenerateCell(x, y - 1);
			if (w) 
				GenerateCell(x + 1, y);
		}
		else if (r < 0.5f) {
			if (w) 
				GenerateCell(x + 1, y);
			if (s) 
				GenerateCell(x, y - 1);
			if (e) 
				GenerateCell(x - 1, y);
			if (n) 
				GenerateCell(x, y + 1);
		}
		else if (r < 0.75f) {
			if (n) 
				GenerateCell(x, y + 1);
			if (s) 
				GenerateCell(x, y - 1);
			if (e) 
				GenerateCell(x - 1, y);
			if (w) 
				GenerateCell(x + 1, y);
		}
		else {
			if (e) 
				GenerateCell(x - 1, y);
			if (n) 
				GenerateCell(x, y + 1);
			if (w) 
				GenerateCell(x + 1, y);
			if (s) 
				GenerateCell(x, y - 1);
		}

	}
}

FVector2D ADungeonGenerator::WorldToCell(FVector location)
{
	int dx = FGenericPlatformMath::FloorToInt((location.X + cell_length / 2.0f) / cell_length);
	int dy = FGenericPlatformMath::FloorToInt((location.Y + cell_length / 2.0f) / cell_length);
	return FVector2D(origin_x + dx, origin_y + dy);
}


void ADungeonGenerator::CalcPlayerCell()
{
	if (playerReference == NULL) return;
	FVector2D location = WorldToCell(playerReference->GetActorLocation());
	playerX = (int)location.X;
	playerY = (int)location.Y;
}


FVector2D ADungeonGenerator::GetPlayerCellLocation()
{
	return FVector2D(playerX, playerY);
}