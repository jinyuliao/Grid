#pragma once

#include "CoreMinimal.h"
#include "GridPawnInterface.generated.h"

class AGridManager;

UINTERFACE(Blueprintable)
class GRIDRUNTIME_API UGridPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
	Your pawn should implement this interface to using grid-based pathfinding and movement function
*/
class GRIDRUNTIME_API IGridPawnInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	AGridManager* GetGridManager();
	virtual AGridManager* GetGridManager_Implementation();
};
