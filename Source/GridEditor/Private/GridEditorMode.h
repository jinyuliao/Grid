#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"
#include "Settings/SquareGridSettings.h"
#include "Settings/HexagonGridSettings.h"

class AGridManager;
class AHexagonGridManager;
class ASquareGridManager;
class UGrid;

class FEdModeGridEditor : public FEdMode
{
public:
	FEdModeGridEditor();
	virtual ~FEdModeGridEditor();

	virtual bool UsesToolkits() const override;
	virtual void Enter() override;
	virtual void Exit() override;
	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;

	virtual void ActorMoveNotify() override;
	virtual void ActorPropChangeNotify() override;
	virtual void ActorSelectionChangeNotify() override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	void SetCurrentMode(FName ModeName);
	FName GetCurrentMode();

	void OnSettingsChanged(const FPropertyChangedEvent& PropertyChangedEvent);

public:
	USquareGridSettings* SquareGridSettings;
	UHexagonGridSettings* HexagonGridSettings;

private:
	void OnNewCurrentLevel();
	void OnMapChanged(uint32 Event);
	void OnBeginPIE(bool bIsSimulating);
	void OnEndPIE(bool bIsSimulating);

	UWorld* GetEditorWorld();

	AGridManager* GetGridManager();
	void FreeGridManager();

	void UpdateGridSettings();
	void UpdateSquareSettings();
	void UpdateHexagonSettings();

private:
	int32 GridShowRange;

	bool bEditorDelegateRegistered;

	FName CurrentModeName;

	TArray<UGrid*> VisibleGrids;

	AHexagonGridManager* HexGridManager;

	ASquareGridManager* SquareGridManager;
};
