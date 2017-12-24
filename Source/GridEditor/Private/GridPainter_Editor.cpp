#include "GridPainter_Editor.h"
#include "GridEditorMode.h"
#include "GridInfo_Editor.h"
#include "GridEditorCommands.h"

UGridPainter_Editor::UGridPainter_Editor()
{
	SquareNormalMaterial = nullptr;
	SquareSensingMaterial = nullptr;
	HexNormalMaterial = nullptr;
	HexSensingMaterial = nullptr;

	bIsSquareGrid = true;
}

UGridPainter_Editor::~UGridPainter_Editor()
{

}

UMaterialInterface* UGridPainter_Editor::GetDecalMaterial_Implementation(UGrid* Grid)
{
	UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);

	if (GridInfo->GetSensing())
	{
		if (bIsSquareGrid)
		{
			return SquareSensingMaterial;
		}
		else
		{
			return HexSensingMaterial;
		}
	}
	else if (GridInfo->GetShowNormal())
	{
		if (bIsSquareGrid)
		{
			return SquareNormalMaterial;
		}
		else
		{
			return HexNormalMaterial;
		}
	}
	return nullptr;
}
