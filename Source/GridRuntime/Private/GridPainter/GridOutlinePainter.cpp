#include "GridPainter/GridOutlinePainter.h"

UGridOutlinePainter::UGridOutlinePainter()
{
	bIsTickable = true;

	GridOutlinePrimitiveComp = CreateDefaultSubobject<UGridOutlinePrimitiveComponent>("CompactGridPrimitiveComp");

	OutlineThickness = 5.f;
	ZDelta = 5.f;
}

UGridOutlinePainter::~UGridOutlinePainter()
{

}

void UGridOutlinePainter::SetGridManager(AGridManager* NewGridManager)
{
	Super::SetGridManager(NewGridManager);

	if (!GridOutlinePrimitiveComp->IsRegistered())
	{
		GridOutlinePrimitiveComp->RegisterComponentWithWorld(GridManager->GetWorld());
	}
}

void UGridOutlinePainter::TickImpl_Implementation(float DeltaTime)
{
	if (bGridStateDirty)
	{
		GridOutlinePrimitiveComp->UpdateGridInfo();

		bGridStateDirty = false;
	}
}

void UGridOutlinePainter::GetColorPriority_Implementation(TArray<FLinearColor>& Colors)
{
	Colors.Reset();
	Colors.Add(FLinearColor::White);
}

void UGridOutlinePainter::GetColors_Implementation(UGrid* Grid, TArray<FLinearColor>& Colors)
{
	Colors.Reset();
	Colors.Add(FLinearColor::White);
}
