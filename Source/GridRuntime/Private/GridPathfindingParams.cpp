#include "GridPathfindingParams.h"
#include "GridManager.h"

FGridPathFindingRequest::FGridPathFindingRequest()
{
	bRemoveDest = false;
	MaxCost = -1;
	MaxSearchStep = 1000;
}

UGridPathFinder::UGridPathFinder()
{

}

UGridPathFinder::~UGridPathFinder()
{
}

UGrid* UGridPathFinder::GetStartGrid() const
{
	check(GridManager != nullptr);

	return GridManager->GetGridByPosition(Request.StartPos);
}

UGrid* UGridPathFinder::GetDestGrid() const
{
	check(GridManager != nullptr);

	return GridManager->GetGridByPosition(Request.DestPos);
}

AActor* UGridPathFinder::GetSender() const
{
	return Request.Sender;
}

void UGridPathFinder::GetStart(FVector& Start) const
{
	Start = Request.StartPos;
}

void UGridPathFinder::GetDestination(FVector& Destination) const
{
	Destination = Request.DestPos;
}

const FGameplayTagContainer& UGridPathFinder::GetExtraTags() const
{
	return Request.ExtraTags;
}

bool UGridPathFinder::IsReachable_Implementation(UGrid* Start, UGrid* Dest)
{
	return !Dest->IsEmpty();
}

int32 UGridPathFinder::Heuristic_Implementation(UGrid* From, UGrid* To)
{
	return GetCost(From, To);
}
