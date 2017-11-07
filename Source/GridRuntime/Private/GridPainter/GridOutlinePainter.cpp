#include "GridPainter/GridOutlinePainter.h"

UGridOutlinePainter::UGridOutlinePainter()
{
	bIsTickable = true;

	GridOutlinePrimitiveComp = CreateDefaultSubobject<UGridOutlinePrimitiveComponent>("CompactGridPrimitiveComp");

	GridOutlinePrimitiveComp->GridPainter = this;

	Thickness = 6.f;
	ZDelta = 10.f;
	OutlineColor = FLinearColor::White;
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
	GridOutlinePrimitiveComp->UpdateGridInfo();
}
