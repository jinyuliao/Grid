#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "SquareGrid.generated.h"

/**
*
*/
UCLASS(Transient)
class GRIDRUNTIME_API USquareGrid : public UGrid
{
	GENERATED_BODY()

public:
	USquareGrid();
	virtual ~USquareGrid();

	virtual FVector GetCenter() const override;

	virtual void SetGridSize(float Size) override;

	virtual void GetNeighbors_Implementation(TArray<UGrid*>& Grids) override;

	UFUNCTION(BlueprintCallable, Category = "SquareGrid")
	virtual void GetSquareNeighbors(TArray<UGrid*>& Grids, bool bDiagonal = false);
};
