#pragma once

#include "CoreMinimal.h"
#include "GridPainter/GridPainter.h"
#include "Components/GridOutlineComponent.h"
#include "GridOutlinePainter.generated.h"

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API UGridOutlinePainter : public UGridPainter
{
	GENERATED_BODY()
	
public:
	UGridOutlinePainter();
	virtual ~UGridOutlinePainter();

	virtual void SetGridManager(AGridManager* NewGridManager) override;
	
	virtual void TickImpl_Implementation(float DeltaTime) override;

	/**
	* Descending order. 
	* @note when different grid with different color share one edge, we need this information to determine the color of the shared edge
	* @note you MUST implement this method if you override 'GetColors'
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridDecalPainter")
	void GetColorPriority(TArray<FLinearColor>& Colors);
	void GetColorPriority_Implementation(TArray<FLinearColor>& Colors);

	/**
	* @note a grid can have multi state(color), e.g: a grid is movable(white) and attackable(red)
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridDecalPainter")
	void GetColors(UGrid* Grid, TArray<FLinearColor>& Colors);
	virtual void GetColors_Implementation(UGrid* Grid, TArray<FLinearColor>& Colors);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridOutlinePainter")
	float OutlineThickness;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridOutlinePainter")
	float ZOffset;

protected:
	UGridOutlineComponent* GridOutlineComp;
};
