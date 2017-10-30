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

		Bound = FBox::BuildAABB(GetCenter(), FVector(GridSize / 2.f));

		GridPainter->UpdateGridState(this);
	}
}
