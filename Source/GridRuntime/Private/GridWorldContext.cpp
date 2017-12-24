#include "GridWorldContext.h"
#include "GridUtilities.h"

UGridWorldContext* UGridWorldContext::Instance = nullptr;

AGridManager* UGridWorldContext::GlobalGridManager = nullptr;

UGridWorldContext::UGridWorldContext()
{

}

UGridWorldContext::~UGridWorldContext()
{
}

void UGridWorldContext::BeginDestroy()
{
	Instance = nullptr;

	Super::BeginDestroy();
}

AGridManager* UGridWorldContext::GetGridManager_Implementation() const
{
	return GlobalGridManager;
}

APawn* UGridWorldContext::GetPawnOnGrid_Implementation(const UGrid* Grid) const
{
	FHitResult HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
	TArray<AActor*> ActorsToIgnore;
	if (UGridUtilities::GridTraceSingleForObjects(Grid, 100.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
	{
		return Cast<APawn>(HitResult.Actor.Get());
	}
	else
	{
		return nullptr;
	}
}

void UGridWorldContext::RegisterGridWorldContext(UGridWorldContext* Context)
{
	if (Instance != nullptr)
	{
		Instance->ConditionalBeginDestroy();
	}
	Instance = Context;
}

UGridWorldContext* UGridWorldContext::GetGridWorldContext()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UGridWorldContext>(UGridWorldContext::StaticClass());
	}
	return Instance;
}

AGridManager* UGridWorldContext::GetGlobalGridManager()
{
	return GlobalGridManager;
}

void UGridWorldContext::SetGlobalGridManager(AGridManager* GridManager)
{
	GlobalGridManager = GridManager;
}
