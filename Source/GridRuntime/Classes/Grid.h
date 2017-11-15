#pragma once

#include "CoreMinimal.h"
#include "GridInfo.h"
#include "Grid.generated.h"

class AGridManager;
class UGridPainter;

UENUM(BlueprintType)
enum class EGridType : uint8
{
	Unknown,
	Square,
	Hexagon
};


/**
*
*/
UCLASS(Transient, Blueprintable)
class GRIDRUNTIME_API UGrid : public UObject
{
	GENERATED_BODY()

public:
	UGrid();
	virtual ~UGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual FVector GetCenter() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual void SetGridSize(float Size);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual float GetGridSize() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool Equal(UGrid* R);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual void SetVisibility(bool NewVisibility);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool GetVisibility() const;

	/**
		if line trace test can't hit any WorldStatic object, then this grid is empty
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool IsEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual FIntVector GetCoord() const;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	EGridType GridType;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	FIntVector Coord;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	FBoxSphereBounds Bounds;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	float Height;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	UGridInfo* GridInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManager;

	void GridInfoChanged();

protected:
	bool bVisible;

	float GridSize;

	friend class AGridManager;
	friend class UGridPainter;
};
