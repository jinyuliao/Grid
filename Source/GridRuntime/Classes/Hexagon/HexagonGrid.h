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

protected:
	// for path finding
	int HeuristicValue;

	friend class AGridManager;
};
