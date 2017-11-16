#include "GridPathfindingParams.h"
#include "GridManager.h"

FGridPathFindingRequest::FGridPathFindingRequest()
{
	bRemoveDest = false;
	MaxStep = -1;
	MaxSearchStep = 1000;
	ExtraFlags = 0;
}

UGridPathFinder::UGridPathFinder()
{

}

UGridPathFinder::~UGridPathFinder()
{
}

UGrid* UGridPathFinder::GetStartGrid()
{
	check(GridManager != nullptr);

	return GridManager->GetGridByPosition(Request.StartPos);
}

UGrid* UGridPathFinder::GetDestGrid()
{
	check(GridManager != nullptr);

	return GridManager->GetGridByPosition(Request.DestPos);
}

AActor* UGridPathFinder::GetSender()
{
	return Request.Sender;
}

bool UGridPathFinder::IsReachable_Implementation(UGrid* Start, UGrid* Dest)
{
	return !Dest->IsEmpty();
}

int32 UGridPathFinder::Heuristic_Implementation(UGrid* From, UGrid* To)
{
	return GetCost(From, To);
}
