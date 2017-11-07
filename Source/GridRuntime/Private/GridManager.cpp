#include "GridManager.h"
#include "GridPainter/GridDecalPainter.h"

template <typename T>
struct SharedUObject
{
	SharedUObject(T* InObj)
	{
		Obj = InObj;
	}

	~SharedUObject()
	{
		if (Obj != nullptr)
		{
			Obj->ConditionalBeginDestroy();
		}
	}

	T* operator->()
	{
		return Obj;
	}

	T* Obj;
};

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

	GridSize = 0.f;
	TraceTestDistance = 10000.f;

	GridInfoClass = UGridInfo::StaticClass();
	GridPainterClass = UGridDecalPainter::StaticClass();
}

AGridManager::~AGridManager()
{

}

void AGridManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PostInitGridManager();
}

void AGridManager::PostInitGridManager()
{
	GridPainter = NewObject<UGridPainter>(this, GridPainterClass);
	check(GridPainter != nullptr);
	GridPainter->PostInitPainter();
	GridPainter->SetGridManager(this);
}

void AGridManager::GetNeighbors_Implementation(UGrid* Center, TArray<UGrid*>& Grids)
{
	Grids.Empty();
}

UGridPainter* AGridManager::GetGridPainter()
{
	return GridPainter;
}

void AGridManager::LineTraceTest(const FVector& Center, TArray<FHitResult>& Results)
{
	FVector Start = Center;
	FVector End = Start;

	Start.Z += TraceTestDistance / 2;
	End.Z -= TraceTestDistance / 2;

	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add((EObjectTypeQuery)ECollisionChannel::ECC_WorldStatic);

	TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::LineTraceMultiForObjects((UObject*)GetWorld(), Start, End, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, Results, true);
}

void AGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Empty();
}

UGrid* AGridManager::GetGridByCoord(const FIntVector& Coord)
{
	TArray<UGrid*> Grids;
	GetGridsByCoord(Coord, Grids);
	return Grids.Num() > 0 ? Grids[0] : nullptr;
}

UGrid* AGridManager::GetGridByPosition(const FVector& Position)
{
	return nullptr;
}

void AGridManager::ClearAllGridInfo()
{

}

void AGridManager::SetGridSize(float NewCellSize)
{
	if (this->GridSize != NewCellSize)
	{
		this->GridSize = NewCellSize;
	}
}

float AGridManager::GetGridSize() const
{
	return GridSize;
}

int AGridManager::GetDistance(const UGrid* Start, const UGrid* Dest)
{
	return 0;
}

bool AGridManager::FindPath(const FGridPathFindingRequest& Request, TArray<UGrid*>& Result)
{
	Result.Empty();	

	bool Succ = false;

	struct FComparer
	{
		bool operator()(const UGrid& L, const UGrid& R) const
		{
			return L.HeuristicValue < R.HeuristicValue;
		}
	};

	FComparer Comparer;
	TArray<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, UGrid*> CameFrom;
	TMap<UGrid*, int> CostSoFar;
	TArray<UGrid*> Neighbors;

	auto PathFinder = SharedUObject<UGridPathFinder>(NewObject<UGridPathFinder>(this, PathFinderClass));
	PathFinder->Request = Request;
	PathFinder->GridManager = this;

	UGrid* Start = GetGridByPosition(Request.StartPos);
	UGrid* Dest = GetGridByPosition(Request.DestPos);
	int MaxFindStep = Request.MaxFindStep;

	CameFrom.Add(Start, nullptr);
	CostSoFar.Add(Start, 0);

	OpenSet.HeapPush(Start, Comparer);

	int Step = 0;

	while (OpenSet.Num() != 0)
	{
		UGrid* Current = nullptr;
		OpenSet.HeapPop(Current, Comparer);

		if (Current->Equal(Dest))
		{
			Succ = true;
			break;
		}

		if (++Step > Request.MaxFindStep)
		{
			LOG_WARNING(TEXT("AGridManager::FindPath Pathfinding failed, run out of MaxFindStep"))
			break;
		}

		CloseSet.Add(Current);

		GetNeighbors(Current, Neighbors);

		for (int i = 0; i < Neighbors.Num(); ++i)
		{
			UGrid* Next = Neighbors[i];

			if (CloseSet.Contains(Next))
				continue;

			if (!PathFinder->IsReachable(Current, Next))
				continue;

			int NewCost = CostSoFar[Current] + PathFinder->GetCost(Current, Next);
			if (!CostSoFar.Contains(Next) || NewCost < CostSoFar[Next])
			{
				CostSoFar.Add(Next, NewCost);

				Next->HeuristicValue = NewCost + PathFinder->GetCost(Current, Next);

				OpenSet.HeapPush(Next, Comparer);

				CameFrom.Add(Next, Current);
			}
		}
	}

	if (Succ)
	{
		Result.Empty();

		UGrid* Current = Dest;
		Result.Add(Current);

		while (Current != Start)
		{
			Current = CameFrom.FindChecked(Current);
			Result.Add(Current);
		}

		Algo::Reverse(Result);

		if (Request.bRemoveDest)
		{
			Result.Pop();
		}

		int MaxStep = Request.MaxStep;
		if (MaxStep >= 0)
		{
			while (Result.Num() > MaxStep + 1)
			{
				Result.Pop();
			}
		}
	}

	return Succ;
}

void AGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

void AGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

