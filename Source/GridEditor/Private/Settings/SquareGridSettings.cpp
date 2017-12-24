#include "SquareGridSettings.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "ConstructorHelpers.h"

USquareGridSettings::USquareGridSettings()
{
	bShowGrids = false;

	GridSize = 100.f;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DecalMat(TEXT("/Grid/Materials/Square/M_SquareGrid_Normal.M_SquareGrid_Normal"));
	DecalMaterial = DecalMat.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> VisualizerMat(TEXT("/Grid/Materials/Square/M_SquareGrid_SensingVisualizer.M_SquareGrid_SensingVisualizer"));
	GridSensingVisualizerMaterial = VisualizerMat.Object;
}

USquareGridSettings::~USquareGridSettings()
{
}

void USquareGridSettings::SetParent(FEdModeGridEditor* GridEditorMode)
{
	ParentMode = GridEditorMode;
}
