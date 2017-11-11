#include "SquareSettingsDetails.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "Widgets/Input/SButton.h"
#include "PropertyCustomizationHelpers.h"
#include "SlateOptMacros.h"
#include "DesktopPlatformModule.h"
#include "SlateApplication.h"

#define LOCTEXT_NAMESPACE "SquareSettingsDetails"

TSharedRef<IDetailCustomization> FSquareSettingsDetails::MakeInstance()
{
	return MakeShareable(new FSquareSettingsDetails);
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FSquareSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& SquareGridCategory = DetailBuilder.EditCategory("SqureGridSettings");
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FEdModeGridEditor* FSquareSettingsDetails::GetEditorMode() const
{
	return (FEdModeGridEditor*)GLevelEditorModeTools().GetActiveMode(EM_GridEditor);
}

#undef LOCTEXT_NAMESPACE
