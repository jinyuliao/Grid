#include "Square/SquareGridManager.h"
#include "Square/SquarePathFinder.h"
#include "Util/GridUtilities.h"

ASquareGridManager::ASquareGridManager()
{
	PathFinderClass = USquarePathFinder::StaticClass();
}

ASquareGridManager::~ASquareGridManager()
{

}

void ASquareGridManager::SetGridSize(float NewGridSize)
{
	this->GridSize = NewGridSize;

	for (auto& Elem : GridsPool)
	{
		FSquareGridArray& GridArray = Elem.Value;
		for (int i = 0; i < GridArray.Grids.Num(); ++i)
		{
			GridArray[i]->SetGridSize(NewGridSize);
		}
	}
}

UGrid* ASquareGridManager::GetGridByPosition(const FVector& Position)
{
	return GetSquareGridByPosition(Position);
}

void ASquareGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	FSquareGridArray GridArray;
	GetSquareGridsByCoord(Coord, GridArray.Grids);
	
	for (int i = 0; i < GridArray.Num(); ++i)
	{
		Grids.AddUnique(GridArray[i]);
	}
}

void ASquareGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	USquareGrid* MinGrid = Cast<USquareGrid>(GetGridByPosition(Bound.Min));
	USquareGrid* MaxGrid = Cast<USquareGrid>(GetGridByPosition(Bound.Max));

	FIntVector MinCoord = MinGrid->GetCoord();
	FIntVector MaxCoord = MaxGrid->GetCoord();

	TArray<UGrid*> TmpGrids;
	for (int i = MinCoord.X; i <= MaxCoord.X; ++i)
	{
		for (int j = MinCoord.Y; j <= MaxCoord.Y; ++j)
		{
			FIntVector CurrCoord(FIntVector(i, j, 0));

			GetGridsByCoord(CurrCoord, TmpGrids);

			Grids.Append(TmpGrids);
		}
	}
}

int ASquareGridManager::GetDistance(const UGrid* Start, const UGrid* Dest)
{
	if (Start == nullptr || Dest == nullptr)
		return TNumericLimits<int32>::Max();

	return FMath::Abs(Start->Coord.X - Dest->Coord.X) + FMath::Abs(Start->Coord.Y - Dest->Coord.Y);
}

void ASquareGridManager::GetNeighbors_Implementation(UGrid* Center, TArray<UGrid*>& Grids)
{
	const FIntVector Directions[] = { FIntVector(1, 0, 0), FIntVector(-1, 0, 0), FIntVector(0, 1, 0), FIntVector(0, -1, 0),};

	Grids.Reset();

	TArray<UGrid*> TmpGrids;

	USquareGrid* Grid = Cast<USquareGrid>(Center);
	for (int i = 0; i < 4; ++i)
	{
		GetGridsByCoord(Grid->Coord + Directions[i], TmpGrids);

		Grids.Append(TmpGrids);
	}
}

void ASquareGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	if (Center == nullptr)
		return;

	FIntVector Coord = Center->Coord;

	TArray<UGrid*> TmpGrids;
	for (int i = -Range; i <= Range; ++i)
	{
		for (int j = -Range; j <= Range; ++j)
		{
			Coord.X = Center->Coord.X + i;
			Coord.Y = Center->Coord.Y + j;

			GetGridsByCoord(Coord, TmpGrids);

			for (int k = 0; k < TmpGrids.Num(); ++k)
			{
				UGrid* Grid = TmpGrids[k];

				if (GetDistance(Grid, Center) <= Range)
				{
					Grids.Add(Grid);
				}
			}
		}
	}

	Grids.Sort([&](const UGrid& L, const UGrid& R)
	{
		return GetDistance(&L, Center) < GetDistance(&R, Center);
	});
}

void ASquareGridManager::ClearAllGridInfo()
{
	for (auto& Elem : GridsPool)
	{
		FSquareGridArray& GridArray = Elem.Value;

		for (int i = 0; i< GridArray.Num(); ++i)
		{
			UGridInfo* GridInfo = GridArray[i]->GridInfo;

			if (GridInfo != nullptr)
			{
				GridInfo->Clear();
			}
		}
	}
}

void ASquareGridManager::GetSquareGridsByCoord(const FIntVector& Coord, TArray<USquareGrid*>& Grids)
{
	Grids.Reset();

	uint64 GridUniqueId = UGridUtilities::GetUniqueIdByCoordinate(Coord);

	FSquareGridArray GridArray;
	if (GridsPool.Contains(GridUniqueId))
	{
		GridArray = GridsPool.FindChecked(GridUniqueId);
		Grids = GridArray.Grids;
	}
	else
	{
		CreateGrids(Coord, GridArray);

		GridsPool.Add(GridUniqueId, GridArray);

		Grids = GridArray.Grids;
	}
}

USquareGrid* ASquareGridManager::GetSquareGridByPosition(const FVector& Position)
{
	FIntVector Coord(FMath::RoundToInt(Position.X / GridSize), FMath::RoundToInt(Position.Y / GridSize), 0);

	TArray<USquareGrid*> Grids;
	GetSquareGridsByCoord(Coord, Grids);

	USquareGrid* Rtn = nullptr;
	
	int MinDistance = TNumericLimits<int32>::Max();
	for (int i = 0; i < Grids.Num(); ++i)
	{
		USquareGrid* Grid = Grids[i];

		int Distance = FMath::Abs(Grid->Height - Position.Z);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Rtn = Grid;
		}
	}

	return Rtn;
}

void ASquareGridManager::CreateGrids(const FIntVector& Coord, FSquareGridArray& GridArray)
{
	TArray<FHitResult> HitResults;
	FVector Center(Coord.X * GridSize, Coord.Y * GridSize, 0.f);

	LineTraceTest(Center, HitResults);

	for (int i = 0; i < HitResults.Num(); ++i)
	{
		if (i != 0)
		{
			// if this static mesh is too close to previous one, it has been blocked, skip this result
			FVector Oringin, Extent;
			HitResults[i - 1].Actor->GetActorBounds(true, Oringin, Extent);

			FBox Bound = FBox::BuildAABB(Oringin, Extent);

			FVector TestPoint = HitResults[i].ImpactPoint;

			TestPoint.Z += 10.f;
			if (Bound.IsInsideOrOn(TestPoint))
				continue;
		}
		GridArray.Add(CreateGrid(Coord, HitResults[i]));
	}

	if (GridArray.Num() == 0)
	{
		// create an empty grid
		GridArray.Add(CreateGrid(Coord, FHitResult()));
	}
}

USquareGrid* ASquareGridManager::CreateGrid(const FIntVector& Coord, const FHitResult& HitResult)
{
	USquareGrid* Grid = NewObject<USquareGrid>(this, USquareGrid::StaticClass());
	check(Grid != nullptr);

	Grid->GridType = EGridType::Square;
	Grid->Coord = Coord;
	Grid->Height = HitResult.ImpactPoint.Z;
	Grid->GridInfo = NewObject<UGridInfo>(Grid, GridInfoClass);
	Grid->GridInfo->HitResult = HitResult;
	Grid->GridInfo->ParentGrid = Grid;
	Grid->GridManager = this;
	Grid->SetGridSize(GridSize);

	GridPainter->UpdateGridState(Grid);

	return Grid;
}
