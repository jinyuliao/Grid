#pragma once

#include "CoreMinimal.h"
#include "IDetailsView.h"
#include "PropertyEditorDelegates.h"
#include "GridEditorModeToolkit.h"

class FEdModeGridEditor;

class SGridEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGridEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FGridEditorToolkit> InParentToolkit);

	void ShowDetails(UObject* Target);

protected:
	FEdModeGridEditor* GetEditorMode() const;

	FText GetErrorText() const;

	bool GetGridEditorIsEnabled() const;

	bool GetIsPropertyVisible(const FPropertyAndParent& PropertyAndParent) const;

protected:
	TSharedPtr<SErrorText> Error;

	TSharedPtr<IDetailsView> DetailsPanel;
};

