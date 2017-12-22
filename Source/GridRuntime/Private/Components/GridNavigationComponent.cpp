#include "Components/GridNavigationComponent.h"
#include "GridRuntimePCH.h"
#include "Grid.h"
#include "GridManager.h"
#include "Components/DefaultGridNavigationAgent.h"
#include "Util/GridUtilities.h"

UGridNavigationComponent::UGridNavigationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerPawn = nullptr;
	OwnerController = nullptr;

	FollowingPathIndex = 0;
	bIsMoving = false;

	NavMode = EGridNavMode::GridBased;

	AgentClasses.Add(UDefaultGridNavigationAgent::StaticClass());
}

UGridNavigationComponent::~UGridNavigationComponent()
{

}

void UGridNavigationComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < AgentClasses.Num(); ++i)
	{
		UGridNavigationAgent* Agent = NewObject<UGridNavigationAgent>(this, AgentClasses[i]);

		if (Agent != nullptr)
		{
			FScriptDelegate Delegate;
			Delegate.BindUFunction(this, "OnMoveCompleted");

			Agent->OnMoveCompleted.Add(Delegate);
			Agents.Add(Agent);
		}
		else
		{
			LOG_ERROR(TEXT("UGridNavigationComponent::BeginPlay create grid navigation agent failed!"));
		}
	}

	OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn != nullptr)
	{
		OwnerController = Cast<AAIController>(OwnerPawn->GetController());
	}
}

bool UGridNavigationComponent::RequestMove(UGrid* DestGrid, UGridPathFinder* PathFinder)
{
	if (OwnerPawn == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::RequestMove failed, OwnerPawn is null"));
		return false;
	}

	if (OwnerController == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::RequestMove failed, OwnerController is null"));
		return false;
	}

	if (DestGrid == nullptr)
	{
		LOG_WARNING(TEXT("UGridNavigationComponent::RequestMove failed, DestGrid is null"));
		return false;
	}

	AGridManager* GridManager = DestGrid->GridManager;

	if (!ensure(GridManager != nullptr))
	{
		return false;
	}

	FGridPathfindingRequest Request;
	TArray<UGrid*> Result;

	Request.Sender = OwnerPawn;
	Request.Destination = DestGrid;
	Request.Start = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());

	if (PathFinder == nullptr)
	{
		PathFinder = GridManager->GetPathFinder();
		PathFinder->Reset();
	}

	if (!UGridUtilities::FindPath(Request, PathFinder, CurrentFollowingPath))
	{
		return false;
	}

	FollowingPathIndex = 0;

	bIsMoving = true;

	MoveToNext();

	return true;
}

bool UGridNavigationComponent::IsMoving() const
{
	return bIsMoving;
}

bool UGridNavigationComponent::MoveToNext()
{
	switch (NavMode)
	{
	case EGridNavMode::GridBased:
		return MoveToNextGrid();
	case EGridNavMode::Free:
		return MoveToNextPoint();
	default:
		return false;
	}
}

bool UGridNavigationComponent::MoveToNextGrid()
{
	++FollowingPathIndex;

	if (FollowingPathIndex >= CurrentFollowingPath.Num())
		return false;

	AGridManager* GridManager = CurrentFollowingPath.Last()->GridManager;

	UGrid* CurrGrid = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());
	UGrid* NextGrid = CurrentFollowingPath[FollowingPathIndex];

	UGridNavigationAgent* Agent = FindAgent(CurrGrid, NextGrid);

	if (Agent == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::MoveToNextGrid can't find proper agent"));
		return false;
	}

	Agent->RequestMove(OwnerPawn, CurrGrid, NextGrid);

	return true;
}

bool UGridNavigationComponent::MoveToNextPoint()
{
	++FollowingPathIndex;

	if (FollowingPathIndex >= CurrentFollowingPath.Num())
		return false;

	AGridManager* GridManager = CurrentFollowingPath.Last()->GridManager;

	UGrid* CurrGrid = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());
	UGrid* NextGrid = CurrentFollowingPath[FollowingPathIndex];

	UGridNavigationAgent* Agent = FindAgent(CurrGrid, NextGrid);
	
	if (Agent == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::MoveToNextGrid can't find proper agent"));
		return false;
	}

	if (Cast<UDefaultGridNavigationAgent>(Agent) != nullptr)
	{
		int i;
		for (i = FollowingPathIndex; i < CurrentFollowingPath.Num() - 1; ++i)
		{
			if (!Agent->Check(OwnerPawn, CurrentFollowingPath[i], CurrentFollowingPath[i + 1]))
				break;
		}

		FollowingPathIndex = i;
		NextGrid = CurrentFollowingPath[FollowingPathIndex];
	}

	Agent->RequestMove(OwnerPawn, CurrGrid, NextGrid);

	return true;
}

UGridNavigationAgent* UGridNavigationComponent::FindAgent(UGrid* Start, UGrid* Goal)
{
	UGridNavigationAgent* Agent = nullptr;
	for (int i = 0; i < Agents.Num(); ++i)
	{
		if (Agents[i]->Check(OwnerPawn, Start, Goal))
		{
			Agent = Agents[i];
			break;
		}
	}
	return Agent;
}

void UGridNavigationComponent::OnMoveCompleted(APawn* Pawn, bool Succ)
{
	if (Succ)
	{
		if (FollowingPathIndex < CurrentFollowingPath.Num() - 1)
		{
			if (NavMode == EGridNavMode::GridBased)
				OnArrivalNewGrid.Broadcast(this);
		}
		else
		{
			OnArrivalGoal.Broadcast(this);
		}

		if (!MoveToNext())
		{
			bIsMoving = false;
		}
	}
	else
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::OnMoveCompleted failed"));

		bIsMoving = false;
	}
}
