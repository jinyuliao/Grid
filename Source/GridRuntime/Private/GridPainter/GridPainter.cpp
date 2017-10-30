#include "GridPainter/GridPainter.h"

UGridPainter::UGridPainter()
{
	GridManager = nullptr;
}

UGridPainter::~UGridPainter()
{

}

void UGridPainter::UpdateGridState_Implementation(UGrid* Grid)
{
	check(Grid != nullptr);

	if (Grid->IsEmpty())
		return;

	if (Grid->bVisible)
	{
		VisibleGrids.AddUnique(Grid);
	}
	else
	{
		VisibleGrids.Remove(Grid);
	}
}
