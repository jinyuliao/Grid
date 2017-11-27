#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridPathfindingParams.generated.h"

class UGrid;
class AGridManager;

USTRUCT(BlueprintType)
struct GRIDRUNTIME_API FGridPathFindingRequest
{
	GENERATED_BODY()

public:
	FGridPathFindingRequest();

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	AActor* Sender;

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	FVector StartPos;

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	FVector DestPos;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	int32 MaxStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	int32 MaxSearchStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	bool bRemoveDest;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	FGameplayTagContainer ExtraTags;
};

/**
	Default pathfinder using manhattan distance to calculate cost.
	Inherit from USquarePathFinder or UHexagonPathFinder to customize pathfinding
*/
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridPathFinder : public UObject
{
	GENERATED_BODY()

public:
	UGridPathFinder();
	virtual ~UGridPathFinder();

	UPROPERTY(BlueprintReadOnly, Category = "GridPathFinder")
	AGridManager* GridManager;

	UPROPERTY(BlueprintReadOnly, Category = "GridPathFinder")
	FGridPathFindingRequest Request;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	UGrid* GetStartGrid();

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	UGrid* GetDestGrid();

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	AActor* GetSender();

	/** If return false, the pawn can't move from 'Start' to 'Dest', the path has been blocked, and the cost will be ignore */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	bool IsReachable(UGrid* Start, UGrid* Dest);
	virtual bool IsReachable_Implementation(UGrid* Start, UGrid* Dest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	int32 GetCost(UGrid* From, UGrid* To);
	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) { return 1; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	int32 Heuristic(UGrid* From, UGrid* To);
	virtual int32 Heuristic_Implementation(UGrid* From, UGrid* To);
};
