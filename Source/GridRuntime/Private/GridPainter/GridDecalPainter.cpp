#include "GridPainter/GridDecalPainter.h"
#include "GridUtilities.h"
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
		Grid2Decal.Add(Grid, DecalComp);
	}

	DecalComp->SetVisibility(Grid->GetVisibility());
	
	DecalComp->SetDecalMaterial(GetDecalMaterial(Grid));

	// update decal size
	int Size = Grid->GetGridSize();
	if (Cast<USquareGrid>(Grid) != nullptr)
	{
		FVector DecalSize(Size, Size, Size);

		DecalSize /= 2;

		DecalSize.X /= 2;

		DecalSize *= 0.98f;

		DecalComp->DecalSize = DecalSize;
	}
	else if (Cast<UHexagonGrid>(Grid) != nullptr)
	{
		FVector DecalSize(Size, Size * 2, Size * FMath::Sqrt(3));

		DecalSize /= 2;

		DecalComp->DecalSize = DecalSize * 0.98f;

		FVector Offset = DecalSize / -2;
		Offset.Z = 0;
		DecalComp->SetRelativeLocation(Offset);
	}
	else
	{
		check(false);
	}

	// update decal location and rotation
	FRotator DestRotator = UKismetMathLibrary::FindLookAtRotation(DecalComp->GetComponentLocation(), Grid->GridInfo->HitResult.ImpactPoint);

	DecalComp->SetWorldLocationAndRotation(Grid->GetCenter(), DestRotator);
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
}

UMaterialInterface* UGridDecalPainter::GetDecalMaterial_Implementation(UGrid* Grid)
{
	return DefaultDecalMaterial;
}

