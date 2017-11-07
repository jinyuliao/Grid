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
									,public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UGridPainter();
	virtual ~UGridPainter();

	virtual void SetGridManager(AGridManager* NewGridManager);

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPainter")
	void UpdateGridState(UGrid* Grid);
	virtual void UpdateGridState_Implementation(UGrid* Grid);

	UFUNCTION(BlueprintNativeEvent, Category = "GridPainter")
	void TickImpl(float DeltaTime);
	virtual void TickImpl_Implementation(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Category = "GridPainter")
	AGridManager* GridManager;

	UPROPERTY(BlueprintReadWrite, Category = "GridPainter")
	TArray<UGrid*> VisibleGrids;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Task")
	bool bIsTickable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Task")
	float TickInterval;

	float LastTickTime;

	TStatId StatId;
};
