#include "GridEditorMode.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "GridEditorModeToolkit.h"
#include "GridPainter/GridDecalPainter.h"

#define LOCTEXT_NAMESPACE "GridEditorMode" 

FEdModeGridEditor::FEdModeGridEditor()
{
	bEditorDelegateRegistered = false;

	HexGridManager = nullptr;
	SquareGridManager = nullptr;

	SquareGridSettings = NewObject<USquareGridSettings>(GetTransientPackage(), TEXT("SquareGridSettings"), RF_Transactional);
	SquareGridSettings->SetParent(this);

	HexagonGridSettings = NewObject<UHexagonGridSettings>(GetTransientPackage(), TEXT("HexagonGridSettings"), RF_Transactional);
	HexagonGridSettings->SetParent(this);
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

	GetGridManager();
}

void FEdModeGridEditor::Exit()
{
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
	FreeGridManager();
	GetGridManager();
	LOG_WARNING(TEXT("Map Changed"));
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
	VisibleGrids.Reset();

	if (HexGridManager != nullptr)
	{
		//HexCellManager->BeginDestroy();
		HexGridManager = nullptr;
	}

	if (SquareGridManager != nullptr)
	{
		//SquareCellManager->BeginDestroy();
		SquareGridManager = nullptr;
	}
}

void FEdModeGridEditor::UpdateGridSettings()
{
	AGridManager* GridManager = GetGridManager();

	UGridDecalPainter* DecalPainter = Cast<UGridDecalPainter>(GridManager->GetGridPainter());

	for (int i = 0; i < VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = VisibleGrids[i];
		Grid->SetVisibility(false);
	}
	VisibleGrids.Reset();

	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		if (DecalPainter != nullptr)
		{
			DecalPainter->DefaultDecalMaterial = SquareGridSettings->DecalMaterial;
		}
		GridManager->SetGridSize(SquareGridSettings->GridSize);

		if (SquareGridSettings->bShowGrids)
		{
			UGrid* Center = GridManager->GetGridByPosition(FVector(0.f, 0.f, 0.f));

			GridManager->GetGridsByRange(Center, SquareGridSettings->ShowGridRange, VisibleGrids);
			for (int i = 0; i < VisibleGrids.Num(); ++i)
			{
				UGrid* Grid = VisibleGrids[i];
				Grid->SetVisibility(true);
			}
		}
	}
	else if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		if (DecalPainter != nullptr)
		{
			DecalPainter->DefaultDecalMaterial = HexagonGridSettings->DecalMaterial;
		}
		GridManager->SetGridSize(HexagonGridSettings->GridSize);

		if (HexagonGridSettings->bShowGrids)
		{
			UGrid* Center = GridManager->GetGridByPosition(FVector(0.f, 0.f, 0.f));

			GridManager->GetGridsByRange(Center, HexagonGridSettings->ShowGridRange, VisibleGrids);
			for (int i = 0; i < VisibleGrids.Num(); ++i)
			{
				UGrid* Grid = VisibleGrids[i];
				Grid->SetVisibility(true);
			}
		}
	}
	
}

#undef LOCTEXT_NAMESPACE
