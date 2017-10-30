#include "GridInfo.h"

UGridInfo::UGridInfo()
{
}

UGridInfo::~UGridInfo()
{

}

void UGridInfo::Clear_Implementation()
{
	HitResult.bBlockingHit = false;
}

void UGridInfo::Dirty()
{
	ParentGrid->GridInfoChanged();
}
