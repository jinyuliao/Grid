#include "Square/SquareGrid.h"

USquareGrid::USquareGrid()
{

}

USquareGrid::~USquareGrid()
{

}

FVector USquareGrid::GetCenter() const
{
	return FVector(Coord.X * GridSize, Coord.Y * GridSize, Height);
}

void USquareGrid::SetGridSize(float Size)
{
	if (FMath::Abs(GridSize - Size) > FLT_EPSILON)
	{
		GridSize = Size;

		Bounds = FBox::BuildAABB(GetCenter(), FVector(GridSize / 2.f)).ExpandBy(FVector::ZeroVector, FVector(0.f, 0.f, GridSize));

		GridManager->GetGridPainter()->UpdateGridState(this);
	}
}

void USquareGrid::GetNeighbors_Implementation(TArray<UGrid*>& Grids)
{
	GetSquareNeighbors(Grids, false);
}

int USquareGrid::GetDistance(const UGrid* Dest) const
{
	if (Dest == nullptr)
		return TNumericLimits<int32>::Max();

	return FMath::Abs(Coord.X - Dest->Coord.X) + FMath::Abs(Coord.Y - Dest->Coord.Y);
}

void USquareGrid::GetSquareNeighbors(TArray<UGrid*>& Grids, bool bDiagonal /*= false*/)
{
	const FIntVector Directions[] = { FIntVector(1, 0, 0), FIntVector(-1, 0, 0), FIntVector(0, 1, 0), FIntVector(0, -1, 0) };

	Grids.Reset();

	TArray<UGrid*> TmpGrids;

	for (int i = 0; i < 4; ++i)
	{
		GridManager->GetGridsByCoord(Coord + Directions[i], TmpGrids);

		Grids.Append(TmpGrids);
	}

	if (bDiagonal)
	{
		const FIntVector DiagonalDirections[] = { FIntVector(1, 1, 0), FIntVector(1, -1, 0), FIntVector(-1, -1, 0), FIntVector(-1, 1, 0) };

		for (int i = 0; i < 4; ++i)
		{
			GridManager->GetGridsByCoord(Coord + DiagonalDirections[i], TmpGrids);

			Grids.Append(TmpGrids);
		}
	}
}
