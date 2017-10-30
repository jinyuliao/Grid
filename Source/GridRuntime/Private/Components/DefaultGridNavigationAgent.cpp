#include "Components/DefaultGridNavigationAgent.h"
#include "GridRuntimePrivatePCH.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"

UDefaultGridNavigationAgent::UDefaultGridNavigationAgent()
{
	AcceptanceRadius = 10.f;
}

UDefaultGridNavigationAgent::~UDefaultGridNavigationAgent()
{

}

bool UDefaultGridNavigationAgent::Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (Pawn == nullptr || From == nullptr || To == nullptr || Cast<AAIController>(Pawn->GetController()) == nullptr)
		return false;

	UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(Pawn->GetWorld());
	UNavigationPath* Path = NavSys->FindPathToLocationSynchronously((UObject*)Pawn->GetWorld(), From->GetCenter(), To->GetCenter(), Pawn);

	if (Path == nullptr || !Path->IsValid() || Path->IsPartial())
		return false;

	return true;
}

bool UDefaultGridNavigationAgent::RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (Pawn == nullptr || From == nullptr || To == nullptr)
		return false;

	CurrPawn = Pawn;
	CurrController = Cast<AAIController>(Pawn->GetController());

	if (CurrController == nullptr)
		return false;

	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, "OnAIControllerMoveCompeleted");
	CurrController->ReceiveMoveCompleted.AddUnique(Delegate);

	EPathFollowingRequestResult::Type Result = CurrController->MoveToLocation(To->GetCenter(), AcceptanceRadius, false);

	bool Succ = false;

	switch (Result)
	{
	case EPathFollowingRequestResult::Type::AlreadyAtGoal:
	case EPathFollowingRequestResult::Type::RequestSuccessful:
		{
			Succ = true;
			break;
		}
	case EPathFollowingRequestResult::Failed:
		{
			FVector Dest = To->GetCenter();
			LOG_ERROR(TEXT("UDefaultGridNavigationAgent::RequestMove_Implementation failed, Destination is (%f, %f, %f)"), Dest.X, Dest.Y, Dest.Z);
			break;
		}
	default:
		break;
	}

	return Succ;
}

void UDefaultGridNavigationAgent::OnAIControllerMoveCompeleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bool Succ = false;

	switch (Result)
	{
	case EPathFollowingResult::Success:
		{
			Succ = true;
			break;
		}
	default:
		{
			LOG_ERROR(TEXT("UDefaultGridNavigationAgent::OnAIControllerMoveCompeleted failed, Result: %d"), (int)Result);
		}
		break;
	}

	APawn* TempPawn = CurrPawn;

	CurrController->ReceiveMoveCompleted.RemoveAll(this);
	CurrPawn = nullptr;
	CurrController = nullptr;

	OnMoveCompleted.Broadcast(TempPawn, Succ);
}
