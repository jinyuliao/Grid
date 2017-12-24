#pragma once

#include "CoreMinimal.h"
#include "GridPainter/GridDecalPainter.h"
#include "GridPainter_Editor.generated.h"

UCLASS()
class UGridPainter_Editor: public UGridDecalPainter
{
	GENERATED_BODY()

public:
	UGridPainter_Editor();
	virtual ~UGridPainter_Editor();

	virtual UMaterialInterface* GetDecalMaterial_Implementation(UGrid* Grid) override;

	UPROPERTY()
	UMaterialInterface* SquareNormalMaterial;

	UPROPERTY()
	UMaterialInterface* SquareSensingMaterial;

	UPROPERTY()
	UMaterialInterface* HexNormalMaterial;

	UPROPERTY()
	UMaterialInterface* HexSensingMaterial;

	bool bIsSquareGrid;
};
