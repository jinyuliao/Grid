#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GridInterface.generated.h"

class AGridManager;

UINTERFACE(Blueprintable)
class GRIDRUNTIME_API UGridPawnInterface : public UInterface
{
	GENERATED_BODY()
};

class GRIDRUNTIME_API IGridPawnInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	AGridManager* GetGridManager() const;
};
