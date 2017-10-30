#include "SquareGridSettings.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "ConstructorHelpers.h"

USquareGridSettings::USquareGridSettings()
{
	bShowGrids = false;

	ShowGridRange = 5;
	GridSize = 100.f;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DecalMat(TEXT("/Grid/Materials/Square/Mat_SquareGrid_Normal.Mat_SquareGrid_Normal"));
	DecalMaterial = DecalMat.Object;
}

USquareGridSettings::~USquareGridSettings()
{
}

void USquareGridSettings::SetParent(FEdModeGridEditor* GridEditorMode)
{
	ParentMode = GridEditorMode;
}
