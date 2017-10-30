#pragma once

#include "CoreMinimal.h"
#include "GridPathfindingParams.generated.h"

class UGrid;
class AGridManager;

USTRUCT(BlueprintType)
struct GRIDRUNTIME_API FGridPathFindingRequest
{
	GENERATED_BODY()

public:
	FGridPathFindingRequest();

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	AActor* Sender;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FVector StartPos;

	UPROPERTY(BlueprintReadWrite, Category = "Grid")
	FVector DestPos;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "Grid")
	int MaxStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "Grid")
	int MaxFindStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "Grid")
	bool bRemoveDest;
};

/**
	Default pathfinder using manhattan distance to calculate cost, you can inherit from USquarePathFinder or UHexagonPathFinder 
	to customize pathfinding
*/
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridPathFinder : public UObject
{
	GENERATED_BODY()

public:
	UGridPathFinder();
	virtual ~UGridPathFinder();

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	AGridManager* GridManager;

	UPROPERTY(BlueprintReadOnly, Category = "Grid")
	FGridPathFindingRequest Request;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	UGrid* GetStartGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	UGrid* GetDestGrid();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	AActor* GetSender();

	/** If return false, the pawn can't move from 'Start' to 'Dest', the path has been blocked, and the cost will be ignore */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	bool IsReachable(UGrid* Start, UGrid* Dest);
	virtual bool IsReachable_Implementation(UGrid* Start, UGrid* Dest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grid")
	int GetCost(UGrid* From, UGrid* To);
	virtual int GetCost_Implementation(UGrid* From, UGrid* To) { return 1; };
};
