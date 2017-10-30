#include "GridEditorModeToolkit.h"
#include "SGridEditor.h"
#include "GridEditorCommands.h"

#define LOCTEXT_NAMESPACE "FGridEditorToolkit"

FGridEditorToolkit::FGridEditorToolkit(FEdModeGridEditor* OwningMode)
{
	this->OwningMode = OwningMode;
}

FGridEditorToolkit::~FGridEditorToolkit()
{

}

void FGridEditorToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	TSharedRef<FUICommandList> CommandList = GetToolkitCommands();

#define MAP_MODE(ModeName) CommandList->MapAction(FGridEditorCommands::NameToCommandMap.FindChecked(ModeName), FUIAction(FExecuteAction::CreateSP(this, &FGridEditorToolkit::OnChangeMode, FName(ModeName)), FCanExecuteAction::CreateSP(this, &FGridEditorToolkit::IsModeEnabled, FName(ModeName)), FIsActionChecked::CreateSP(this, &FGridEditorToolkit::IsModeActive, FName(ModeName))));
	MAP_MODE("ToolMode_Square");
	MAP_MODE("ToolMode_Hexagon");
#undef MAP_MODE

	GridEditorWidgets = SNew(SGridEditor, SharedThis(this));

	FModeToolkit::Init(InitToolkitHost);
}

FName FGridEditorToolkit::GetToolkitFName() const
{
	return TEXT("GridEditorToolkit");
}

FText FGridEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("GridEditorToolkitBaseToolkitName", "GridEditorToolkit");
}

class FEdMode* FGridEditorToolkit::GetEditorMode() const
{
	return OwningMode;
}

TSharedPtr<SWidget> FGridEditorToolkit::GetInlineContent() const
{
	return GridEditorWidgets;
}

void FGridEditorToolkit::OnChangeMode(FName ModeName)
{
	OwningMode->SetCurrentMode(ModeName);

	if (ModeName == FGridEditorCommands::SquareModeName)
	{
		GridEditorWidgets->ShowDetails(OwningMode->SquareGridSettings);
	}
	else if (ModeName == FGridEditorCommands::HexagonModeName)
	{
		GridEditorWidgets->ShowDetails(OwningMode->HexagonGridSettings);
	}
	else
	{
		GridEditorWidgets->ShowDetails(nullptr);
	}
}

bool FGridEditorToolkit::IsModeEnabled(FName ModeName) const
{
	return true;
}

bool FGridEditorToolkit::IsModeActive(FName ModeName) const
{
	return OwningMode->GetCurrentMode() == ModeName;
}

#undef LOCTEXT_NAMESPACE
