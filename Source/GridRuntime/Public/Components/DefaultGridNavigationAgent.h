#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Grid.h"
#include "Components/GridNavigationAgent.h"
#include "DefaultGridNavigationAgent.generated.h"

/**
 *
 */
UCLASS()
class GRIDRUNTIME_API UDefaultGridNavigationAgent : public UGridNavigationAgent
{
	GENERATED_BODY()
	
public:
	UDefaultGridNavigationAgent();
	virtual ~UDefaultGridNavigationAgent();

	virtual bool Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To) override;

	virtual bool RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To) override;

	virtual void StopMove_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultGridNavigationAgent")
	float AcceptanceRadius;

protected:
	UFUNCTION()
	virtual void OnAIControllerMoveCompeleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UPROPERTY()
	APawn* CurrentPawn;

	UPROPERTY()
	AAIController* CurrentController;
};
