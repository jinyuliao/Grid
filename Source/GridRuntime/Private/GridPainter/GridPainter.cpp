#include "GridPainter/GridPainter.h"

UGridPainter::UGridPainter()
{
	GridManager = nullptr;
	bIsTickable = true;
	TickInterval = 0.1f;
	LastTickTime = 0.f;
}

UGridPainter::~UGridPainter()
{

}

void UGridPainter::PostInitPainter()
{

}

void UGridPainter::SetGridManager(AGridManager* NewGridManager)
{
	GridManager = NewGridManager;
}

void UGridPainter::Tick(float DeltaTime)
{
	if (GridManager == nullptr)
		return;

	const float WorldTime = GridManager->GetWorld()->GetTimeSeconds();

	if (WorldTime - LastTickTime >= TickInterval)
	{
		LastTickTime = WorldTime;
		TickImpl(DeltaTime);
	}
}

bool UGridPainter::IsTickable() const
{
	return bIsTickable;
}

TStatId UGridPainter::GetStatId() const
{
	return StatId;
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

void UGridPainter::TickImpl_Implementation(float DeltaTime)
{

}
