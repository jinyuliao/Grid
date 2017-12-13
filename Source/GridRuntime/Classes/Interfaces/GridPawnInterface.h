#pragma once

#include "CoreMinimal.h"
#include "Interface.h"
#include "GridPawnInterface.generated.h"

class AGridManager;

UINTERFACE(Blueprintable)
class GRIDRUNTIME_API UGridPawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
	implement this interface to use grid-based pathfinding and movement
*/
class GRIDRUNTIME_API IGridPawnInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	AGridManager* GetGridManager();
	virtual AGridManager* GetGridManager_Implementation();
};
