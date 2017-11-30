#include "Hexagon/HexagonGrid.h"

UHexagonGrid::UHexagonGrid()
{
}

UHexagonGrid::~UHexagonGrid()
{

}

FVector UHexagonGrid::GetCenter() const
{
	float X = GridSize * FMath::Sqrt(3.f) * (Coord.X + Coord.Z / 2.f);
	float Y = GridSize * 3.f / 2.f * Coord.Z;
	return FVector(X, Y, Height);
}

void UHexagonGrid::SetGridSize(float Size)
{
	if (FMath::Abs(GridSize - Size) > FLT_EPSILON)
	{
		GridSize = Size;

		FVector Center = GetCenter();
		FVector Points[6];
		for (int i = 0; i < 6; ++i)
		{
			float Radian = PI / 180 * (i * 60 + 30);
			Points[i] = Center + FVector(GridSize * FMath::Cos(Radian), GridSize * FMath::Sin(Radian), 0.f);
		}

		Bounds = FBox(Points, 6).ExpandBy(FVector::ZeroVector, FVector(0.f, 0.f, GridSize));

		GridManager->GetGridPainter()->UpdateGridState(this);
	}
}

int UHexagonGrid::GetDistance(const UGrid* Dest) const
{
	if (Dest == nullptr)
		return TNumericLimits<int32>::Max();
	return (FMath::Abs(Coord.X - Dest->Coord.X) + FMath::Abs(Coord.Y - Dest->Coord.Y) + FMath::Abs(Coord.Z - Dest->Coord.Z)) / 2;
}

void UHexagonGrid::GetNeighbors_Implementation(TArray<UGrid*>& Grids)
{
	const FIntVector Directions[] = { FIntVector(1, -1, 0), FIntVector(1, 0, -1), FIntVector(0, 1, -1), FIntVector(-1, 1, 0), FIntVector(-1, 0, 1), FIntVector(0, -1, 1) };

	Grids.Reset();

	TArray<UGrid*> TmpGrids;
	for (int i = 0; i < 6; ++i)
	{
		GridManager->GetGridsByCoord(Coord + Directions[i], TmpGrids);
		Grids.Append(TmpGrids);
	}
}
