#pragma once

#include "CoreMinimal.h"
#include "GridPathfindingParams.h"
#include "SquarePathFinder.generated.h"

UCLASS()
class GRIDRUNTIME_API USquarePathFinder : public UGridPathFinder
{
	GENERATED_BODY()

public:
	USquarePathFinder() {};
	virtual ~USquarePathFinder() {};

	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) override;
};
