#include "Components/GridNavigationComponent.h"
#include "GridRuntimePrivatePCH.h"
#include "Grid.h"
#include "GridManager.h"
#include "Interfaces/GridPawnInterface.h"
#include "Components/DefaultGridNavigationAgent.h"

UGridNavigationComponent::UGridNavigationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerPawn = nullptr;
	OwnerController = nullptr;

	FollowingPathIndex = 0;
	bIsMoving = false;

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

bool UGridNavigationComponent::RequestMove(UGrid* DestGrid)
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

	if (!OwnerPawn->GetClass()->ImplementsInterface(UGridPawnInterface::StaticClass()))
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::RequestMove failed, owner hasn't implement IGridPawnInterface"));
		return false;
	}

	AGridManager* GridManager = IGridPawnInterface::Execute_GetGridManager(OwnerPawn);

	if (GridManager == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::RequestMove failed, grid manager is null, maybe your pawn class didn't implement GridPawnInterface?"));
		return false;
	}

	FGridPathFindingRequest Request;
	TArray<UGrid*> Result;

	Request.Sender = OwnerPawn;
	Request.DestPos = DestGrid->GetCenter();
	Request.StartPos = OwnerPawn->GetActorLocation();

	bool Succ = GridManager->FindPath(Request, CurrentFollowingPath);

	if (!Succ)
	{
		return false;
	}

	FollowingPathIndex = 0;

	bIsMoving = true;

	MoveToNextGrid();

	return true;
}

bool UGridNavigationComponent::IsMoving()
{
	return bIsMoving;
}

bool UGridNavigationComponent::MoveToNextGrid()
{
	++FollowingPathIndex;

	if (FollowingPathIndex >= CurrentFollowingPath.Num())
		return false;

	AGridManager* GridManager = IGridPawnInterface::Execute_GetGridManager(OwnerPawn);

	UGrid* CurrGrid = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());
	UGrid* NextGrid = CurrentFollowingPath[FollowingPathIndex];

	UGridNavigationAgent* Agent = nullptr;
	for (int i = 0; i < Agents.Num(); ++i)
	{
		if (Agents[i]->Check(OwnerPawn, CurrGrid, NextGrid))
		{
			Agent = Agents[i];
			break;
		}
	}

	if (Agent == nullptr)
	{
		LOG_ERROR(TEXT("UGridNavigationComponent::MoveToNextGrid can't find proper agent"));
		return false;
	}

	Agent->RequestMove(OwnerPawn, CurrGrid, NextGrid);

	return true;
}

void UGridNavigationComponent::OnMoveCompleted(APawn* Pawn, bool Succ)
{
	if (Succ)
	{
		if (FollowingPathIndex < CurrentFollowingPath.Num() - 1)
		{
			OnArrivalNewGrid.Broadcast(this);
		}
		else
		{
			OnArrivalGoal.Broadcast(this);
		}

		if (!MoveToNextGrid())
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
