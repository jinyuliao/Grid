#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "GridPathfindingParams.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GridUtilities.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Grid-based pathfinding */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static bool FindPath(const FGridPathfindingRequest& Request, UGridPathFinder* PathFinder, TArray<UGrid*>& Result);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static bool GetReachableGrids(AActor* Sender, int32 MaxCost, UGridPathFinder* PathFinder, TArray<UGrid*>& Result);

	UFUNCTION(BlueprintPure, Category = "Math", meta = (DisplayName = "IntVector - IntVector", CompactNodeTitle = "-", Keywords = "- sub minus"))
	static FIntVector Subtract_IntVectorIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintPure, Category = "Math", meta = (DisplayName = "IntVector + IntVector", CompactNodeTitle = "+", Keywords = "+ add plus"))
	static FIntVector Add_IntVectorIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintPure, Category = "Math", meta = (DisplayName = "IntVector * integer", CompactNodeTitle = "*", Keywords = "* multiply"))
	static FIntVector Multiply_IntVectorInt(const FIntVector& L, int R);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (Grid)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Utilities")
	static bool EqualEqual_GridGrid(const UGrid* A, const UGrid* B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Not Equal (Grid)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "Utilities")
	static bool NotEqual_GridGrid(const UGrid* A, const UGrid* B);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static bool GridTraceSingleForObjects(UGrid* Grid, float TraceDistance, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static void GetBoundsByGridArray(const TArray<UGrid*>& Grids, FBoxSphereBounds& Bounds);

	static uint64 GetUniqueIdByCoordinate(const FIntVector& Coord);

	static FVector CalcGridDecalSize(EGridType GridType, float GridSize);
};
