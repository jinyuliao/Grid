#pragma once

#include "CoreMinimal.h"
#include "Square/SquareGrid.h"
#include "GridManager.h"
#include "SquareGridManager.generated.h"

USTRUCT()
struct FSquareGridArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<USquareGrid*> Grids;

	void Add(USquareGrid* Grid) { Grids.Add(Grid); }

	size_t Num() { return Grids.Num(); }

	USquareGrid* operator[](std::size_t idx) { return Grids[idx]; }

	const USquareGrid* operator[](std::size_t idx) const { return Grids[idx]; }
};

/**
*
*/
UCLASS(Transient)
class GRIDRUNTIME_API ASquareGridManager : public AGridManager
{
	GENERATED_BODY()

public:
	ASquareGridManager();
	virtual ~ASquareGridManager();

	virtual void SetGridSize(float CellSize) override;

	virtual int GetDistance(const UGrid* Start, const UGrid* Dest) override;

	virtual void GetNeighbors_Implementation(UGrid* Center, TArray<UGrid*>& Grids) override;

	virtual UGrid* GetGridByPosition(const FVector& Position) override;

	virtual void GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Cells) override;

	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids) override;

	virtual void ClearAllGridInfo() override;

	UFUNCTION(BlueprintCallable, Category = "SquareGridManager")
	void GetSquareGridsByCoord(const FIntVector& Coord, TArray<USquareGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "SquareGridManager")
	USquareGrid* GetSquareGridByPosition(const FVector& Position);

protected:
	void CreateGrids(const FIntVector& Coord, FSquareGridArray& GridArray);
	USquareGrid* CreateGrid(const FIntVector& Coord, const FHitResult& HitResult);

protected:
	UPROPERTY()
	TMap<uint64, FSquareGridArray> GridsPool;
};
