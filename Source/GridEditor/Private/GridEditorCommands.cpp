#include "GridEditorCommands.h"

#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "GridEditorCommands" 

FName FGridEditorCommands::SquareModeName = "ToolMode_Square";

FName FGridEditorCommands::HexagonModeName = "ToolMode_Hexagon";

TMap<FName, TSharedPtr<FUICommandInfo> > FGridEditorCommands::NameToCommandMap;

FGridEditorCommands::FGridEditorCommands()
	: TCommands<FGridEditorCommands>("GridEditor", NSLOCTEXT("Contexts", "GridEditor", "Grid Editor"), NAME_None, FEditorStyle::GetStyleSetName())
{

}

FGridEditorCommands::~FGridEditorCommands()
{

}

void FGridEditorCommands::RegisterCommands()
{
	UI_COMMAND(SquareMode, "Mode - Square", "", EUserInterfaceActionType::RadioButton, FInputChord());
	NameToCommandMap.Add(SquareModeName, SquareMode);
	UI_COMMAND(HexagonMode, "Mode - Hexagon", "", EUserInterfaceActionType::RadioButton, FInputChord());
	NameToCommandMap.Add(HexagonModeName, HexagonMode);
}

#undef LOCTEXT_NAMESPACE
