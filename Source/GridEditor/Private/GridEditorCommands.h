#pragma once

#include "CoreMinimal.h"

class FGridEditorCommands : public TCommands<FGridEditorCommands>
{
public:
	FGridEditorCommands();
	virtual ~FGridEditorCommands();

	virtual void RegisterCommands() override;

public:
	// Mode Switch
	TSharedPtr<FUICommandInfo> SquareMode;
	TSharedPtr<FUICommandInfo> HexagonMode;

	static FName SquareModeName;
	static FName HexagonModeName;

	static TMap<FName, TSharedPtr<FUICommandInfo> > NameToCommandMap;
};
