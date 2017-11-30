#pragma once

#include "CoreMinimal.h"
#include "HexagonGrid.generated.h"

/**
 * 
 */
UCLASS(Transient)
class GRIDRUNTIME_API UHexagonGrid : public UGrid
{
	GENERATED_BODY()
	
public:
	UHexagonGrid();
	virtual ~UHexagonGrid();

	virtual FVector GetCenter() const override;

	virtual void SetGridSize(float Size) override;

	virtual int GetDistance(const UGrid* Dest) const override;

	virtual void GetNeighbors_Implementation(TArray<UGrid*>& Grids) override;
};
