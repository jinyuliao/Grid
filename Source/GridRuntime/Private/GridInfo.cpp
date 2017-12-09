#include "GridInfo.h"
#include "GridManager.h"

UGridInfo::UGridInfo()
{
}

UGridInfo::~UGridInfo()
{

}

void UGridInfo::Clear_Implementation()
{
	HitResult.bBlockingHit = false;
	GameplayTags.Reset();
	Dirty();
}

void UGridInfo::Dirty()
{
	ParentGrid->GridManager->GetGridPainter()->UpdateGridState(ParentGrid);
}
