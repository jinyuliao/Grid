#pragma once

#include "CoreMinimal.h"
#include "SquareGridSettings.generated.h"

class FEdModeGridEditor;

UCLASS()
class USquareGridSettings : public UObject
{
	GENERATED_BODY()

public:
	USquareGridSettings();
	virtual ~USquareGridSettings();

	void SetParent(FEdModeGridEditor* GridEditorMode);

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	bool bShowGrids;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	float GridSize;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	UMaterialInterface* GridSensingVisualizerMaterial;

private:
	FEdModeGridEditor* ParentMode;
};
