#include "GridInfo_Editor.h"

UGridInfo_Editor::UGridInfo_Editor()
{
	bShowNormal = false;
	bSensing = false;
}

bool UGridInfo_Editor::GetVisibility() const
{
	return bShowNormal || bSensing;
}

void UGridInfo_Editor::SetShowNormal(bool bNewShowNormal)
{
	bShowNormal = bNewShowNormal;

	Dirty();
}

void UGridInfo_Editor::SetSensing(bool bNewSensing)
{
	bSensing = bNewSensing;

	Dirty();
}

