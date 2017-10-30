#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridNavigationAgent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridNavAgentEvent, APawn*, Pawn, bool, Succ);

/**
 * GridNavigationAgent is used to control the pawn's movement behavior, you can inherit from this class to implement special movement(jump, climb, etc)
 */
UCLASS(Blueprintable, abstract)
class GRIDRUNTIME_API UGridNavigationAgent : public UObject
{
	GENERATED_BODY()
	
public:
	UGridNavigationAgent();
	virtual ~UGridNavigationAgent();

	/** check if this agent can process this move */
	UFUNCTION(BlueprintNativeEvent, Category = "GridNavigationAgent")
	bool Check(APawn* Pawn, UGrid* From, UGrid* To);
	virtual bool Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To);

	/** if you override RequestMove, when the pawn arrival destination, you should call OnMoveCompleted */
	UFUNCTION(BlueprintNativeEvent, Category = "GridNavigationAgent")
	bool RequestMove(APawn* Pawn, UGrid* From, UGrid* To);
	virtual bool RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "GridNavigationAgent")
	FGridNavAgentEvent OnMoveCompleted;
};
