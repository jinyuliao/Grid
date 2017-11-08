#pragma once

#include "CoreMinimal.h"
#include "GridPainter/GridPainter.h"
#include "GridPainter/GridOutlinePrimitiveComponent.h"
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridDecalPainter")
	void GetColorPriority(TArray<FLinearColor>& Colors);
	void GetColorPriority_Implementation(TArray<FLinearColor>& Colors);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridDecalPainter")
	void GetColors(UGrid* Grid, TArray<FLinearColor>& Colors);
	virtual void GetColors_Implementation(UGrid* Grid, TArray<FLinearColor>& Colors);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridOutlinePainter")
	float OutlineThickness;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridOutlinePainter")
	float ZDelta;

protected:
	UGridOutlinePrimitiveComponent* GridOutlinePrimitiveComp;
};
