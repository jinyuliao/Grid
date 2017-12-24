#pragma once

#include "CoreMinimal.h"
#include "HexagonGridSettings.generated.h"

class FEdModeGridEditor;

UCLASS()
class UHexagonGridSettings : public UObject
{
	GENERATED_BODY()

public:
	UHexagonGridSettings();
	virtual ~UHexagonGridSettings();

	void SetParent(FEdModeGridEditor* GridEditorMode);

	UPROPERTY(EditAnywhere, Category = "HexagonGridSettings")
	bool bShowGrids;

	UPROPERTY(EditAnywhere, Category = "HexagonGridSettings")
	bool bGridSnap;

	UPROPERTY(EditAnywhere, Category = "HexagonGridSettings")
	float GridSize;

	UPROPERTY(EditAnywhere, Category = "HexagonGridSettings")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere, Category = "HexagonGridSettings")
	UMaterialInterface* GridSensingVisualizerMaterial;

private:
	FEdModeGridEditor* ParentMode;
};
