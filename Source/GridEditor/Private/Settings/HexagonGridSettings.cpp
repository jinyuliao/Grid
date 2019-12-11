#include "HexagonGridSettings.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstance.h"
#include "UObject/ConstructorHelpers.h"

UHexagonGridSettings::UHexagonGridSettings()
{
	bShowGrids = false;
	bGridSnap = false;

	GridSize = 100.f;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> DecalMat(TEXT("/Grid/Materials/Hexagon/M_HexagonGrid_Normal.M_HexagonGrid_Normal"));
	DecalMaterial = DecalMat.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> VisualizerMat(TEXT("/Grid/Materials/Hexagon/M_HexagonGrid_SensingVisualizer.M_HexagonGrid_SensingVisualizer"));
	GridSensingVisualizerMaterial = VisualizerMat.Object;
}

UHexagonGridSettings::~UHexagonGridSettings()
{
}

void UHexagonGridSettings::SetParent(FEdModeGridEditor* GridEditorMode)
{
	ParentMode = GridEditorMode;
}
