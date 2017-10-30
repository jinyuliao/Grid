#include "SGridEditor.h"
#include "GridEditorMode.h"
#include "SlateOptMacros.h"
#include "PropertyEditorModule.h"

extern const FEditorModeID EM_GridEditor;

#define LOCTEXT_NAMESPACE "GridEditor"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGridEditor::Construct(const FArguments& InArgs, TSharedRef<FGridEditorToolkit> InParentToolkit)
{
	TSharedRef<FUICommandList> CommandList = InParentToolkit->GetToolkitCommands();

	// Modes:
	FToolBarBuilder ModeSwitchButtons(CommandList, FMultiBoxCustomization::None);
	{
		ModeSwitchButtons.AddToolBarButton(FGridEditorCommands::Get().SquareMode, NAME_None, LOCTEXT("Mode.Square", "Squarer"), LOCTEXT("Mode.Square.Tooltip", "Square mode"));
		ModeSwitchButtons.AddToolBarButton(FGridEditorCommands::Get().HexagonMode, NAME_None, LOCTEXT("Mode.Hexagon", "Hexagon"), LOCTEXT("Mode.Hexagon.Tooltip", "Hexagon mode"));
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);

	FEdModeGridEditor* GridEditorMode = GetEditorMode();

	DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsPanel->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateSP(this, &SGridEditor::GetIsPropertyVisible));
	DetailsPanel->OnFinishedChangingProperties().AddSP(GridEditorMode, &FEdModeGridEditor::OnSettingsChanged);

	if (GridEditorMode != nullptr)
	{
		DetailsPanel->SetObject(GridEditorMode->SquareGridSettings);
	}

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 5)
			[
				SAssignNew(Error, SErrorText)
			]
			+ SVerticalBox::Slot()
			.Padding(0)
			[
				SNew(SVerticalBox)
				.IsEnabled(this, &SGridEditor::GetGridEditorIsEnabled)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4, 0, 4, 5)
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.HAlign(HAlign_Center)
						[
							ModeSwitchButtons.MakeWidget()
						]
					]
				]
				+ SVerticalBox::Slot()
				.Padding(0)
				[
					DetailsPanel.ToSharedRef()
				]
			]
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SGridEditor::ShowDetails(UObject* Target)
{
	DetailsPanel->SetObject(Target);
}

FEdModeGridEditor* SGridEditor::GetEditorMode() const
{
	return (FEdModeGridEditor*)GLevelEditorModeTools().GetActiveMode(EM_GridEditor);
}

FText SGridEditor::GetErrorText() const
{
	return FText::GetEmpty();
}

bool SGridEditor::GetGridEditorIsEnabled() const
{
	FEdModeGridEditor* GridEditorMode = GetEditorMode();
	if (GridEditorMode != nullptr)
	{
		Error->SetError(GetErrorText());
		return true;
	}
	return false;
}

bool SGridEditor::GetIsPropertyVisible(const FPropertyAndParent& PropertyAndParent) const
{
	const UProperty& Property = PropertyAndParent.Property;

	FEdModeGridEditor* GridEditorMode = GetEditorMode();
	if (GridEditorMode != nullptr)
	{
		//TODO: do some check
	}

	return true;
}

#undef LOCTEXT_NAMESPACE
