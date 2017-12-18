#include "GridPathfindingParams.h"
#include "GridManager.h"

FGridPathfindingRequest::FGridPathfindingRequest()
{
	bRemoveDest = false;
	MaxCost = -1;
	MaxSearchStep = 1000;
	Start = Destination = nullptr;
}

UGridPathFinder::UGridPathFinder()
{
	GridManager = nullptr;
}

UGridPathFinder::~UGridPathFinder()
{
}

UGrid* UGridPathFinder::GetStart() const
{
	return Request.Start;
}

UGrid* UGridPathFinder::GetDestination() const
{
	return Request.Destination;
}

AActor* UGridPathFinder::GetSender() const
{
	return Request.Sender;
}

AGridManager* UGridPathFinder::GetGridManager() const
{
	return GridManager;
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
