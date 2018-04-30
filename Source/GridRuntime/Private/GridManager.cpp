#include "GridManager.h"
#include "GridRuntimePCH.h"
#include "GridPainter/GridDecalPainter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/Queue.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

	GridPainter = nullptr;
	GridSize = 0.f;
	TraceTestDistance = 10000.f;

	PathFinder = nullptr;

	GridInfoClass = UGridInfo::StaticClass();
	GridPainterClass = UGridDecalPainter::StaticClass();
}

AGridManager::~AGridManager()
{

}

void AGridManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PostInitGridManager();
}

void AGridManager::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGridManager::PostInitGridManager()
{
	SetGridPainter(GridPainterClass);

	PathFinder = NewObject<UGridPathFinder>(this, PathFinderClass);
	PathFinder->GridManager = this;
}

void AGridManager::SetGridPainter(TSubclassOf<UGridPainter> PainterClass)
{
	if (GridPainter != nullptr)
		GridPainter->ConditionalBeginDestroy();

	GridPainter = NewObject<UGridPainter>(this, GridPainterClass);
	check(GridPainter != nullptr);
	GridPainter->PostInitPainter();
	GridPainter->SetGridManager(this);
}

UGridPainter* AGridManager::GetGridPainter()
{
	return GridPainter;
}

void AGridManager::LineTraceTest(const FVector& Center, TArray<FHitResult>& Results)
{
	FVector Start = Center;
	FVector End = Start;

	Start.Z += TraceTestDistance / 2;
	End.Z -= TraceTestDistance / 2;

	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };

	TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::LineTraceMultiForObjects((UObject*)GetWorld(), Start, End, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, Results, true);
}

void AGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Empty();
}

UGrid* AGridManager::GetGridByCoord(const FIntVector& Coord)
{
	TArray<UGrid*> Grids;
	GetGridsByCoord(Coord, Grids);
	return Grids.Num() > 0 ? Grids[0] : nullptr;
}

UGrid* AGridManager::GetGridByPosition(const FVector& Position)
{
	return nullptr;
}

void AGridManager::ClearAllGridInfo()
{

}

UGridPathFinder* AGridManager::GetPathFinder() const
{
	return PathFinder;
}

void AGridManager::SetGridSize(float NewCellSize)
{
	if (this->GridSize != NewCellSize)
	{
		this->GridSize = NewCellSize;
	}
}

float AGridManager::GetGridSize() const
{
	return GridSize;
}

void AGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

void AGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

