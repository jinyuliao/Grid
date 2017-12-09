#pragma once

#include "CoreMinimal.h"
#include "Grid.h"
#include "GridPathfindingParams.h"
#include "HexagonPathFinder.generated.h"

UCLASS()
class GRIDRUNTIME_API UHexagonPathFinder : public UGridPathFinder
{
	GENERATED_BODY()

public:
	UHexagonPathFinder();
	virtual ~UHexagonPathFinder();

	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) override;
};
