#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Grid.h"
#include "GridPainter.generated.h"

/**
 * GridPainter is used for rendering grid in game world, the default painter is GridDecalPainter.
   you can inherit from this class to customize grid rendering
 */
UCLASS(Blueprintable, abstract)
class GRIDRUNTIME_API UGridPainter : public UObject
{
	GENERATED_BODY()
	
public:
	UGridPainter();
	virtual ~UGridPainter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPainter")
	void UpdateGridState(UGrid* Grid);
	virtual void UpdateGridState_Implementation(UGrid* Grid);

	UPROPERTY(BlueprintReadOnly, Category = "GridPainter")
	AGridManager* GridManager;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "GridPainter")
	TArray<UGrid*> VisibleGrids;
};
