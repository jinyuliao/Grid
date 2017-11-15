#pragma once

#include "CoreMinimal.h"
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
	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector SubIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector AddIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector IntVectorMulInt(const FIntVector& L, int R);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static bool GridTraceSingleForObjects(UGrid* Grid, float TraceDistance, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor = FLinearColor::Red, FLinearColor TraceHitColor = FLinearColor::Green, float DrawTime = 5.0f);

	static uint64 GetUniqueIdByCoordinate(const FIntVector& Coord);

	static FVector CalcGridDecalSize(EGridType GridType, float GridSize);
};
