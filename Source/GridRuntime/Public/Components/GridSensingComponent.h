#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"
#include "GridSensingComponent.generated.h"

class AGridManager;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSeePawnEvent, APawn*, Pawn);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSenseTestFunc, APawn*, Pawn, bool&, CouldSense);

/**
*/
UCLASS(ClassGroup = (Grid), Blueprintable, meta = (BlueprintSpawnableComponent))
class GRIDRUNTIME_API UGridSensingComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGridSensingComponent();
	virtual ~UGridSensingComponent();

	void InitializeComponent() override;

	/** interval == 0 disables sensing  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridSensingComponent", meta = (UIMin = "0.0"))
	float SensingInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSensingComponent", meta = (UIMin = "0"))
	int32 VisionGridRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSensingComponent", meta = (UIMin = "0.0", UIMax = "180.0"))
	float VisionAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSensingComponent")
	bool bSkipLineSightTest;

	/** Valid when using SquareGridManager */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSensingComponent")
	bool bDiagonal;

	UPROPERTY(BlueprintReadOnly, Category = "GridSensingComponent")
	TArray<APawn*> SensedPawns;

	UPROPERTY(BlueprintReadWrite, Category = "GridSensingComponent")
	FSenseTestFunc SenseTestFunc;

	UPROPERTY(BlueprintAssignable, Category = "GridSensingComponent")
	FSeePawnEvent OnSeePawn;

	/** interval <= 0 disables sensing  */
	UFUNCTION(BlueprintCallable, Category = "GridSensingComponent")
	virtual void SetSensingInterval(float NewInterval);

	UFUNCTION(BlueprintCallable, Category = "GridSensingComponent")
	virtual bool CouldSeePawn(const APawn* Pawn) const;

	UFUNCTION(BlueprintCallable, Category = "GridSensingComponent")
	virtual void GetSensingGrids(TArray<UGrid*>& SensingGrids) const;

protected:
	UFUNCTION()
	void DefaultSenseTestFunc(APawn* Pawn, bool& CouldSense);

	void SetSensingTimer(float Interval);
	void OnSensingTimer();

	void UpdateSensing();

	AController* GetSensorController() const;

	virtual void GetSensingGridsInternal(AGridManager* GridManager, TArray<UGrid*>& SensingGrids) const;

protected:
	FTimerHandle TimerHandle_Sensing;

	friend class FGridSensingComponentVisualizer;
};
