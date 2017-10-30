#include "GridEditorCommands.h"
#include "GridEditorPrivatePCH.h"
#include "GridEditorMode.h"

DEFINE_LOG_CATEGORY(GridEditor)

#define LOCTEXT_NAMESPACE "GridEditorModule" 

const FEditorModeID EM_GridEditor(TEXT("EM_GridEditor"));

class FGridEditor : public IGridEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FGridEditor, GridEditor)

void FGridEditor::StartupModule()
{
	FGridEditorCommands::Register();

	FEditorModeRegistry::Get().RegisterMode<FEdModeGridEditor>(EM_GridEditor, NSLOCTEXT("EditorModes", "GridEditorMode", "GridEditor"), FSlateIcon(), true, 1000);
}


void FGridEditor::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(EM_GridEditor);
}

#undef LOCTEXT_NAMESPACE
