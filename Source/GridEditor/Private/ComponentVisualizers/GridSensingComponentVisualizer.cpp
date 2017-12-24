#include "GridSensingComponentVisualizer.h"
#include "GridEditorMode.h"
#include "GridManager.h"
#include "GridInfo_Editor.h"
#include "Editor.h"
#include "EditorModeManager.h"

extern const FEditorModeID EM_GridEditor;

FGridSensingComponentVisualizer::FGridSensingComponentVisualizer()
{
	bEnabled = true;
	GridEditor = nullptr;
}

void FGridSensingComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	ClearSensingGrids();

	if (bEnabled && View->Family->EngineShowFlags.VisualizeSenses)
	{
		if (GridEditor == nullptr)
		{
			FEditorModeTools& Tools = GLevelEditorModeTools();
			Tools.ActivateMode(EM_GridEditor);
			return;
		}

		const UGridSensingComponent* GridSensingComp = Cast<const UGridSensingComponent>(Component);

		if (ensure(GridSensingComp != nullptr))
		{
			GridSensingComp->GetSensingGridsInternal(GridEditor->GetGridManager(), SensingGrids);

			for (int32 i = 0; i < SensingGrids.Num(); ++i)
			{
				UGrid* Grid = SensingGrids[i];
				UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
				GridInfo->SetSensing(true);
				Grid->SetVisibility(GridInfo->GetVisibility());
			}
		}
	}
}

void FGridSensingComponentVisualizer::ClearSensingGrids()
{
	for (int32 i = 0; i < SensingGrids.Num(); ++i)
	{
		UGrid* Grid = SensingGrids[i];
		UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
		GridInfo->SetSensing(false);
		Grid->SetVisibility(GridInfo->GetVisibility());
	}
	SensingGrids.Reset();
}

void FGridSensingComponentVisualizer::SetEnabled(bool bNewEnabled)
{
	bEnabled = bNewEnabled;
	if (!bEnabled)
	{
		ClearSensingGrids();
	}
}
