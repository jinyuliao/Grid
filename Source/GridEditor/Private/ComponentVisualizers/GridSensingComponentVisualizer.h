#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"
#include "SceneManagement.h"
#include "SceneView.h"
#include "Components/GridSensingComponent.h"

class UGrid;

class FGridSensingComponentVisualizer : public FComponentVisualizer
{
public:
	FGridSensingComponentVisualizer();

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

	void ClearSensingGrids();

	void SetEnabled(bool bNewEnabled);

	UMaterialInterface* SquareMat;

	UMaterialInterface* HexagonMat;

	TArray<UGrid*> SensingGrids;

	class FEdModeGridEditor* GridEditor;

protected:
	bool bEnabled;
};
