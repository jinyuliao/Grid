#pragma once

#include "CoreMinimal.h"
#include "GridManager.h"
#include "Hexagon/HexagonGrid.h"
#include "Hexagon/HexagonPathFinder.h"
#include "HexagonGridManager.generated.h"

USTRUCT()
struct FHexagonGridArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UHexagonGrid*> Grids;

	void Add(UHexagonGrid* Grid) { Grids.Add(Grid); }

	size_t Num() { return Grids.Num(); }

	UHexagonGrid* operator[](std::size_t idx) { return Grids[idx]; }

	const UHexagonGrid* operator[](std::size_t idx) const { return Grids[idx]; }
};

/**
 * reference: https://www.redblobgames.com/grids/hexagons/
 */
UCLASS(Transient)
class GRIDRUNTIME_API AHexagonGridManager : public AGridManager
{
	GENERATED_BODY()
	
public:
	AHexagonGridManager();
	virtual ~AHexagonGridManager();

	virtual void SetGridSize(float CellSize) override;

	virtual UGrid* GetGridByPosition(const FVector& Position) override;

	virtual void GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids) override;

	virtual int GetDistance(const UGrid* Start, const UGrid* Dest) override;

	virtual void GetNeighbors_Implementation(UGrid* Center, TArray<UGrid*>& Grids) override;

	virtual void ClearAllGridInfo() override;

	UFUNCTION(BlueprintCallable, Category = "Hexagon")
	void GetHexagonGridsByCoord(const FIntVector& Coord, TArray<UHexagonGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "Hexagon")
	UHexagonGrid* GetHexagonGridByPosition(const FVector& Postion);

protected:
	void CreateGrids(const FIntVector& Coord, FHexagonGridArray& GridArray);
	UHexagonGrid* CreateGrid(const FIntVector& Coord, const FHitResult& HitResult);

	FIntVector CubeRound(float X, float Y, float Z);

protected:
	UPROPERTY()
	TMap<uint64, FHexagonGridArray> GridsPool;
};
