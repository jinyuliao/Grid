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

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	bool bShowGrids;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	bool bGridSnap;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	int ShowGridRange;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	float GridSize;

	UPROPERTY(EditAnywhere, Category = "SqureGridSettings")
	UMaterialInterface* DecalMaterial;

private:
	FEdModeGridEditor* ParentMode;
};
