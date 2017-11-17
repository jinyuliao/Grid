#pragma once

#include "CoreMinimal.h"
#include "GridPainter/GridPainter.h"
#include "GridDecalPainter.generated.h"

/**
 * Default grid painter, using decal to visualize grid
 */
UCLASS()
class GRIDRUNTIME_API UGridDecalPainter : public UGridPainter
{
	GENERATED_BODY()
	
public:
	UGridDecalPainter();
	virtual ~UGridDecalPainter();
	
	virtual void TickImpl_Implementation(float DeltaTime);

	virtual void UpdateGridState_Implementation(UGrid* Grid) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GridDecalPainter")
	UMaterialInterface* DefaultDecalMaterial;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridDecalPainter")
	UMaterialInterface* GetDecalMaterial(UGrid* Grid);
	virtual UMaterialInterface* GetDecalMaterial_Implementation(UGrid* Grid);

protected:
	virtual void UpdateDecal(UGrid* Grid);

protected:
	UPROPERTY()
	TMap<UGrid*, UDecalComponent*> Grid2Decal;
};
