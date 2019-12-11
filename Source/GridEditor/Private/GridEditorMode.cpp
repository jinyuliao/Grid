#include "GridEditorMode.h"
#include "GridEditorPCH.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "GridEditorModeToolkit.h"
#include "GridEditorCommands.h"
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"
#include "Components/GridSensingComponent.h"
#include "ComponentVisualizers/GridSensingComponentVisualizer.h"
#include "GridPainter_Editor.h"
#include "GridInfo_Editor.h"

#define LOCTEXT_NAMESPACE "GridEditorMode" 

FEdModeGridEditor::FEdModeGridEditor()
{
	bEditorDelegateRegistered = false;

	HexGridManager = nullptr;
	SquareGridManager = nullptr;

	GridShowRange = 50;

	SquareGridSettings = NewObject<USquareGridSettings>(GetTransientPackage(), TEXT("SquareGridSettings"), RF_Transactional);
	SquareGridSettings->SetParent(this);

	HexagonGridSettings = NewObject<UHexagonGridSettings>(GetTransientPackage(), TEXT("HexagonGridSettings"), RF_Transactional);
	HexagonGridSettings->SetParent(this);

	GetSensingCompVisualizer()->GridEditor = this;
}

FEdModeGridEditor::~FEdModeGridEditor()
{
	if (bEditorDelegateRegistered)
	{
		FEditorDelegates::NewCurrentLevel.RemoveAll(this);
		FEditorDelegates::MapChange.RemoveAll(this);
		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);
	}
}

bool FEdModeGridEditor::UsesToolkits() const
{
	return true;
}

void FEdModeGridEditor::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FGridEditorToolkit(this));
		Toolkit->Init(Owner->GetToolkitHost());
	}

	SetCurrentMode(FGridEditorCommands::SquareModeName);

	if (!bEditorDelegateRegistered)
	{
		FEditorDelegates::NewCurrentLevel.AddSP(this, &FEdModeGridEditor::OnNewCurrentLevel);
		FEditorDelegates::MapChange.AddSP(this, &FEdModeGridEditor::OnMapChanged);
		FEditorDelegates::BeginPIE.AddSP(this, &FEdModeGridEditor::OnBeginPIE);
		FEditorDelegates::EndPIE.AddSP(this, &FEdModeGridEditor::OnEndPIE);

		bEditorDelegateRegistered = true;
	}

	GetSensingCompVisualizer()->SetEnabled(true);
}

void FEdModeGridEditor::Exit()
{
	GetSensingCompVisualizer()->SetEnabled(false);

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	FEdMode::Exit();
}

void FEdModeGridEditor::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);
}

void FEdModeGridEditor::ActorMoveNotify()
{
	AActor* Actor = GetFirstSelectedActorInstance();

	if (Actor != nullptr && CurrentModeName == FGridEditorCommands::HexagonModeName && HexagonGridSettings->bGridSnap)
	{
		AGridManager* CellManager = GetGridManager();
		UGrid* Grid = CellManager->GetGridByPosition(Actor->GetActorLocation());
		
		if (Grid != nullptr)
		{
			FVector NewLocation = Actor->GetActorLocation();
			FVector GridCenter = Grid->GetCenter();
			NewLocation.X = GridCenter.X;
			NewLocation.Y = GridCenter.Y;

			Actor->SetActorLocation(NewLocation);
		}
	}
}

void FEdModeGridEditor::ActorPropChangeNotify()
{
}

void FEdModeGridEditor::ActorSelectionChangeNotify()
{
	AActor* SelectedActor = GetFirstSelectedActorInstance();
	if (SelectedActor == nullptr)
		return;

	UGridSensingComponent* SensingComp = Cast<UGridSensingComponent>(SelectedActor->GetComponentByClass(UGridSensingComponent::StaticClass()));
	if (SensingComp == nullptr)
	{
		GetSensingCompVisualizer()->ClearSensingGrids();
	}
}

void FEdModeGridEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdMode::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(SquareGridSettings);
	Collector.AddReferencedObject(HexagonGridSettings);
}

void FEdModeGridEditor::SetCurrentMode(FName ModeName)
{
	CurrentModeName = ModeName;

	AGridManager* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());
	DecalPainter->bIsSquareGrid = CurrentModeName == FGridEditorCommands::SquareModeName;
}

FName FEdModeGridEditor::GetCurrentMode()
{
	return CurrentModeName;
}

void FEdModeGridEditor::OnSettingsChanged(const FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateGridSettings();
}

void FEdModeGridEditor::OnNewCurrentLevel()
{
}

void FEdModeGridEditor::OnMapChanged(uint32 Event)
{
	GetSensingCompVisualizer()->SensingGrids.Reset();

	VisibleGrids.Reset();

	FreeGridManager();
}

void FEdModeGridEditor::OnBeginPIE(bool bIsSimulating)
{
}

void FEdModeGridEditor::OnEndPIE(bool bIsSimulating)
{
}

UWorld* FEdModeGridEditor::GetEditorWorld()
{
	return GEditor->GetEditorWorldContext().World();
}

AGridManager* FEdModeGridEditor::GetGridManager()
{
	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		if (SquareGridManager == nullptr)
		{
			SquareGridManager = GetEditorWorld()->SpawnActor<ASquareGridManager>();
			SquareGridManager->GridPainterClass = UGridPainter_Editor::StaticClass();
			SquareGridManager->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			SquareGridManager->PostInitGridManager();
			UpdateGridSettings();
		}

		return SquareGridManager;
	}
	else if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		if (HexGridManager == nullptr)
		{
			HexGridManager = GetEditorWorld()->SpawnActor<AHexagonGridManager>();
			HexGridManager->GridPainterClass = UGridPainter_Editor::StaticClass();
			HexGridManager->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			HexGridManager->PostInitGridManager();
			UpdateGridSettings();
		}

		return HexGridManager;
	}
	else
	{
		return nullptr;
	}
}

void FEdModeGridEditor::FreeGridManager()
{
	ClearVisibleGrids();

	HexGridManager = nullptr;

	SquareGridManager = nullptr;
}

void FEdModeGridEditor::ClearVisibleGrids()
{
	for (int i = 0; i < VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = VisibleGrids[i];
		UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
		GridInfo->SetShowNormal(false);
		Grid->SetVisibility(GridInfo->GetVisibility());
	}
	VisibleGrids.Reset();
}

void FEdModeGridEditor::UpdateGridSettings()
{
	ClearVisibleGrids();

	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		UpdateSquareSettings();
	}
	else if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		UpdateHexagonSettings();
	}
}

void FEdModeGridEditor::UpdateSquareSettings()
{
	AGridManager* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareNormalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareSensingMaterial = SquareGridSettings->GridSensingVisualizerMaterial;
	}
	GridManager->SetGridSize(SquareGridSettings->GridSize);

	if (SquareGridSettings->bShowGrids)
	{
 		FEditorViewportClient* Viewport = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

		UGrid* Center = GridManager->GetGridByPosition(Viewport->GetViewLocation());

		GridManager->GetGridsByRange(Center, GridShowRange, VisibleGrids);
		for (int i = 0; i < VisibleGrids.Num(); ++i)
		{
			UGrid* Grid = VisibleGrids[i];
			UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
			GridInfo->SetShowNormal(true);
			Grid->SetVisibility(GridInfo->GetVisibility());
		}
	}
}

void FEdModeGridEditor::UpdateHexagonSettings()
{
	AGridManager* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexNormalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexSensingMaterial = HexagonGridSettings->GridSensingVisualizerMaterial;
	}
	GridManager->SetGridSize(HexagonGridSettings->GridSize);

	if (HexagonGridSettings->bShowGrids)
	{
		FEditorViewportClient* Viewport = (FEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();

		UGrid* Center = GridManager->GetGridByPosition(Viewport->GetViewLocation());

		GridManager->GetGridsByRange(Center, GridShowRange, VisibleGrids);
		for (int i = 0; i < VisibleGrids.Num(); ++i)
		{
			UGrid* Grid = VisibleGrids[i];
			UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
			GridInfo->SetShowNormal(true);
			Grid->SetVisibility(GridInfo->GetVisibility());
		}
	}
}

TSharedPtr<FGridSensingComponentVisualizer> FEdModeGridEditor::GetSensingCompVisualizer() const
{
	return StaticCastSharedPtr<FGridSensingComponentVisualizer>(GUnrealEd->FindComponentVisualizer(UGridSensingComponent::StaticClass()->GetFName()));
}

#undef LOCTEXT_NAMESPACE
