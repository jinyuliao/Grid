#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "Components/GridNavigationAgent.h"
#include "GridNavigationComponent.generated.h"

class UGrid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridNavEventSignature, UGridNavigationComponent*, Component);

UCLASS( ClassGroup=(Grid), Blueprintable, meta=(BlueprintSpawnableComponent) )
class GRIDRUNTIME_API UGridNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGridNavigationComponent();
	virtual ~UGridNavigationComponent();

	virtual void BeginPlay() override;

	/**
	*	@note make sure your character's "Max Acceleration" large enough for smooth moving
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool RequestMove(UGrid* DestGrid);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	virtual bool IsMoving();

	UPROPERTY(BlueprintAssignable, Category = "Grid")
	FGridNavEventSignature OnArrivalNewGrid;

	UPROPERTY(BlueprintAssignable, Category = "Grid")
	FGridNavEventSignature OnArrivalGoal;

	/** if you implement a new GridNavigationAgent, you should add that class to this Array */
	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	TArray<TSubclassOf<UGridNavigationAgent> > AgentClasses;

protected:
	virtual bool MoveToNextGrid();

	UFUNCTION()
	virtual void OnMoveCompleted(APawn* Pawn, bool Succ);

protected:
	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	AAIController* OwnerController;

	int FollowingPathIndex;

	bool bIsMoving;

	UPROPERTY()
	TArray<UGrid*> CurrentFollowingPath;

	UPROPERTY()
	TArray<UGridNavigationAgent*> Agents;
};
