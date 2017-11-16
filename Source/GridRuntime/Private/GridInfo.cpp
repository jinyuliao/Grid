#include "GridInfo.h"

UGridInfo::UGridInfo()
{
	ExtraFlag = 0;
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

void UGridInfo::SetExtraFlag(int32 Index)
{
	ExtraFlag |= (1 << Index);

	Dirty();
}

void UGridInfo::ClearExtraFlag(int32 Index)
{
	ExtraFlag &= ~(1 << Index);

	Dirty();
}

void UGridInfo::ToggleExtraFlag(int32 Index)
{
	ExtraFlag ^= (1 << Index);

	Dirty();
}

bool UGridInfo::CheckExtraFlag(int32 Index)
{
	return (ExtraFlag >> Index) & 1;
}
