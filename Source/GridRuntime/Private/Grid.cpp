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

	GridInfo = nullptr;
	GridManager = nullptr;
	GridType = EGridType::Unknown;
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

	GridManager->GetGridPainter()->UpdateGridState(this);
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
		GridManager->GetGridPainter()->UpdateGridState(this);
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
	GridManager->GetGridPainter()->UpdateGridState(this);
}
