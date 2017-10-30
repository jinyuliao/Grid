#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridUtilities.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridUtilities : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector SubIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector AddIntVector(const FIntVector& L, const FIntVector& R);

	UFUNCTION(BlueprintCallable, Category = "GridUtilities")
	static FIntVector IntVectorMulInt(const FIntVector& L, int R);

	static uint64 GetUniqueIdByCoordinate(const FIntVector& Coord);
};
