#include "GridPainter/GridOutlinePainter.h"

UGridOutlinePainter::UGridOutlinePainter()
{
	bIsTickable = true;

	GridOutlinePrimitiveComp = CreateDefaultSubobject<UGridOutlinePrimitiveComponent>("CompactGridPrimitiveComp");

	Thickness = 5.f;
	ZDelta = 5.f;
	Margin = 0.f;
	OutlineCompBoundScale = 1.f;
}

UGridOutlinePainter::~UGridOutlinePainter()
{

}

void UGridOutlinePainter::PostInitPainter()
{
	Super::PostInitPainter();

	GridOutlinePrimitiveComp->SetBoundsScale(OutlineCompBoundScale);
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

FLinearColor UGridOutlinePainter::GetColor_Implementation(UGrid* Grid)
{
	return FLinearColor::White;
}
