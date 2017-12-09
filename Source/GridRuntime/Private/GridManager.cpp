#include "GridManager.h"
#include "GridRuntimePCH.h"
#include "GridPainter/GridDecalPainter.h"
#include "Kismet/KismetSystemLibrary.h"

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

	GridPainter = nullptr;
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
	SetGridPainter(GridPainterClass);
}

void AGridManager::SetGridPainter(TSubclassOf<UGridPainter> PainterClass)
{
	if (GridPainter != nullptr)
		GridPainter->ConditionalBeginDestroy();

	GridPainter = NewObject<UGridPainter>(this, GridPainterClass);
	check(GridPainter != nullptr);
	GridPainter->PostInitPainter();
	GridPainter->SetGridManager(this);
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

class FAStar
{
public:
	FAStar(UGrid* _Start, UGrid* _Goal, UGridPathFinder* _PathFinder, bool _Reversed = false)
		: Start(_Start), Goal(_Goal), PathFinder(_PathFinder), Reversed(_Reversed), Succ(false)
	{
		Comparer.FCost = &FCost;

		GCost.Add(Start, 0);

		FCost.Add(Start, PathFinder->GetCost(Start, Goal));

		OpenSet.HeapPush(Start, Comparer);
	}

	bool Step()
	{
		if (OpenSet.Num() == 0)
			return false;

		UGrid* Current = nullptr;
		OpenSet.HeapPop(Current, Comparer);

		if (Current->Equal(Goal))
		{
			Succ = true;
			return true;
		}

		CloseSet.Add(Current);

		TArray<UGrid*> Neighbors;
		Current->GetNeighbors(Neighbors);

		for (int i = 0; i < Neighbors.Num(); ++i)
		{
			UGrid* Next = Neighbors[i];

			if (CloseSet.Contains(Next) || !(Reversed ? PathFinder->IsReachable(Next, Current) : PathFinder->IsReachable(Current, Next)))
				continue;

			int NewCost = GCost[Current] + (Reversed ? PathFinder->GetCost(Next, Current) : PathFinder->GetCost(Current, Next));
			if (!GCost.Contains(Next) || NewCost < GCost[Next])
			{
				GCost.Add(Next, NewCost);

				FCost.Add(Next, NewCost + (Reversed ? PathFinder->Heuristic(Goal, Next) : PathFinder->Heuristic(Next, Goal)));

				if (!OpenSet.Contains(Next))
					OpenSet.HeapPush(Next, Comparer);

				CameFrom.Add(Next, Current);
			}
		}
		return true;
	}

	UGrid* GetIntersect(const FAStar& R)
	{
		for (UGrid* Grid : CloseSet)
		{
			if (R.CloseSet.Contains(Grid))
				return Grid;
		}

		for (UGrid* Grid : R.CloseSet)
		{
			if (CloseSet.Contains(Grid))
				return Grid;
		}
		return nullptr;
	}

	void CollectPath(UGrid* StartGrid, TArray<UGrid*>& Result)
	{
		UGrid* CurrentGrid = StartGrid;
		Result.AddUnique(CurrentGrid);

		while (CurrentGrid != Start)
		{
			CurrentGrid = CameFrom.FindChecked(CurrentGrid);
			Result.AddUnique(CurrentGrid);
		}
	}

	bool Succ;
private:
	struct FComparer
	{
		bool operator()(const UGrid& L, const UGrid& R) const
		{
			int32 LFCost = FCost->Contains(&L) ? FCost->FindChecked(&L) : TNumericLimits<int32>::Max();
			int32 RFCost = FCost->Contains(&R) ? FCost->FindChecked(&R) : TNumericLimits<int32>::Max();
			return LFCost < RFCost;
		}
		TMap<UGrid*, int32>* FCost;
	} Comparer;

	bool Reversed;
	TArray<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> FCost;
	TMap<UGrid*, int32> GCost;
	TMap<UGrid*, UGrid*> CameFrom;
	UGrid* Start;
	UGrid* Goal;
	UGridPathFinder* PathFinder;

	friend class FBidirectionalAStar;
};

class FBidirectionalAStar
{
public:
	FBidirectionalAStar(UGrid* _Start, UGrid* _Goal, UGridPathFinder* _PathFinder)
		:ForwardAStar(_Start, _Goal, _PathFinder), BackwardAStar(_Goal, _Start, _PathFinder, true), Succ(false), IntersectGrid(nullptr)
	{}

	bool Step()
	{
		if (!ForwardAStar.Step())
			return false;

		if (ForwardAStar.Succ)
		{
			IntersectGrid = ForwardAStar.Goal;
		}
		else
		{
			if (!BackwardAStar.Step())
				return false;

			if (BackwardAStar.Succ)
				IntersectGrid = BackwardAStar.Start;
		}

		if (IntersectGrid == nullptr)
			IntersectGrid = ForwardAStar.GetIntersect(BackwardAStar);
		
		if (IntersectGrid != nullptr)
		{
			Succ = true;
		}
		return true;
	}

	void CollectPath(TArray<UGrid*>& Result)
	{
		ForwardAStar.CollectPath(IntersectGrid, Result);
		Algo::Reverse(Result);
		BackwardAStar.CollectPath(IntersectGrid, Result);
	}

	bool Succ;
private:
	UGrid* IntersectGrid;
	FAStar ForwardAStar;
	FAStar BackwardAStar;
};

bool AGridManager::FindPath(const FGridPathFindingRequest& Request, TArray<UGrid*>& Result)
{
	Result.Reset();	

	bool Succ = false;

	auto PathFinder = SharedUObject<UGridPathFinder>(NewObject<UGridPathFinder>(this, PathFinderClass));
	PathFinder->Request = Request;
	PathFinder->GridManager = this;

	UGrid* Start = GetGridByPosition(Request.StartPos);
	UGrid* Goal = GetGridByPosition(Request.DestPos);

	FBidirectionalAStar BidirectionalAStar(Start, Goal, PathFinder.Obj);

	int32 Step = 0;
	while (!Succ)
	{
		if (!BidirectionalAStar.Step())
			break;

		Succ = BidirectionalAStar.Succ;

		if (++Step > Request.MaxSearchStep)
		{
			LOG_WARNING(TEXT("AGridManager::FindPath failed, out of MaxFindStep"));
			break;
		}
	}

	if (Succ)
	{
		BidirectionalAStar.CollectPath(Result);

		if (Request.bRemoveDest)
		{
			Result.Pop();
		}

		if (Request.MaxCost >= 0)
		{
			int32 Cost = 0;
			int32 i;
			for (i = 1; i < Result.Num(); ++i)
			{
				Cost += PathFinder->GetCost(Result[i - 1], Result[i]);

				if (Cost > Request.MaxCost)
					break;
			}

			if (i < Result.Num())
			{
				Result.RemoveAt(i, Result.Num() - i);
				Succ = false;
			}
		}
	}
	return Succ;
}

bool AGridManager::GetReachableGrids(AActor* Sender, int32 MaxCost, FGameplayTagContainer ExtraTags, TArray<UGrid*>& Result)
{
	Result.Reset();

	if (Sender == nullptr || MaxCost < 0)
		return false;

	auto PathFinder = SharedUObject<UGridPathFinder>(NewObject<UGridPathFinder>(this, PathFinderClass));
	PathFinder->Request.Sender = Sender;
	PathFinder->Request.MaxCost = MaxCost;
	PathFinder->Request.ExtraTags = ExtraTags;
	PathFinder->GridManager = this;

	TQueue<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> Cost;
	UGrid* Start = GetGridByPosition(Sender->GetActorLocation());

	OpenSet.Enqueue(Start);
	Result.Add(Start);

	Cost.Add(Start, 0);

	while (!OpenSet.IsEmpty())
	{
		UGrid* Current;
		OpenSet.Dequeue(Current);

		CloseSet.Add(Current);

		TArray<UGrid*> Neighbors;
		Current->GetNeighbors(Neighbors);

		for (int i = 0; i < Neighbors.Num(); ++i)
		{
			UGrid* Next = Neighbors[i];

			PathFinder->Request.DestPos = Next->GetCenter();

			if (CloseSet.Contains(Next) || !PathFinder->IsReachable(Current, Next))
				continue;

			int NewCost = Cost[Current] + PathFinder->GetCost(Current, Next);

			if (NewCost > MaxCost)
				continue;

			Result.AddUnique(Next);

			if (!Cost.Contains(Next) || NewCost < Cost[Next])
			{
				Cost.Add(Next, NewCost);

				OpenSet.Enqueue(Next);
			}
		}
	}

	return Result.Num() > 0;
}

void AGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

void AGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

