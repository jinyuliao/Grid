#include "GridPainter/GridDecalPainter.h"
#include "Util/GridUtilities.h"
#include "Square/SquareGrid.h"
#include "Hexagon/HexagonGrid.h"
#include "Kismet/KismetMathLibrary.h"

UGridDecalPainter::UGridDecalPainter()
{
	DefaultDecalMaterial = nullptr;
}

UGridDecalPainter::~UGridDecalPainter()
{

}

void UGridDecalPainter::UpdateGridState_Implementation(UGrid* Grid)
{
	Super::UpdateGridState_Implementation(Grid);

	if (!Grid->GridInfo->HitResult.bBlockingHit)
		return;

	UDecalComponent* DecalComp;

	if (Grid2Decal.Contains(Grid))
	{
		DecalComp = Grid2Decal.FindChecked(Grid);
	}
	else
	{
		DecalComp = NewObject<UDecalComponent>(this);
		DecalComp->RegisterComponentWithWorld(GridManager->GetWorld());
		DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		Grid2Decal.Add(Grid, DecalComp);
	}

	DecalComp->SetVisibility(Grid->GetVisibility());
	
	DecalComp->SetDecalMaterial(GetDecalMaterial(Grid));

	DecalComp->DecalSize = UGridUtilities::CalcGridDecalSize(Grid->GridType, Grid->GetGridSize()) * 0.98f;

	DecalComp->SetWorldLocation(Grid->GetCenter());
	
}

UMaterialInterface* UGridDecalPainter::GetDecalMaterial_Implementation(UGrid* Grid)
{
	return DefaultDecalMaterial;
}

