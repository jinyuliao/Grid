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

		GridPainter->UpdateGridState(this);
	}
}
