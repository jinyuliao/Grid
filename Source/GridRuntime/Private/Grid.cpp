#include "Grid.h"
#include "GridManager.h"
#include "GridPainter/GridPainter.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGrid::UGrid()
{
	GridSize = 0.f;
	Height = 0.f;

	bVisible = false;
	HeuristicValue = 0;

	GridInfo = nullptr;
	GridManager = nullptr;
	GridPainter = nullptr;
}

UGrid::~UGrid()
{

}

FVector UGrid::GetCenter() const
{
	return FVector::ZeroVector;
}

void UGrid::SetGridSize(float Size)
{
	GridSize = Size;

	GridPainter->UpdateGridState(this);
}

float UGrid::GetGridSize() const
{
	return GridSize;
}

bool UGrid::Equal(UGrid* R)
{
	return Coord == R->Coord && FMath::Abs(Height - R->Height) <= FLT_EPSILON;
}

void UGrid::SetVisibility(bool Visibility)
{
	if (bVisible != Visibility)
	{
		bVisible = Visibility;
		GridPainter->UpdateGridState(this);
	}
}

bool UGrid::GetVisibility() const
{
	return bVisible;
}

bool UGrid::IsEmpty() const
{
	return !GridInfo->HitResult.bBlockingHit;
}

FIntVector UGrid::GetCoord() const
{
	return Coord;
}

void UGrid::GridInfoChanged()
{
	GridPainter->UpdateGridState(this);
}
