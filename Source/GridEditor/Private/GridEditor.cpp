#include "GridEditorCommands.h"
#include "GridEditorPCH.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "GridEditorMode.h"
#include "SquareSettingsDetails.h"
#include "Components/GridSensingComponent.h"
#include "ComponentVisualizers/GridSensingComponentVisualizer.h"
#include "EditorModeManager.h"

DEFINE_LOG_CATEGORY(GridEditor)

const FEditorModeID EM_GridEditor(TEXT("EM_GridEditor"));

#define LOCTEXT_NAMESPACE "GridEditorModule" 

class FGridEditor : public IGridEditor
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	* Registers a custom class
	*
	* @param ClassName				The class name to register for property customization
	* @param DetailLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate);

	/**
	* Registers a custom struct
	*
	* @param StructName				The name of the struct to register for property customization
	* @param StructLayoutDelegate	The delegate to call to get the custom detail layout instance
	*/
	void RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate);

	void RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer);
	void RegisterComponentVisualizer();
	void UnregisterComponentVisualizer();

private:
	/** List of registered class that we must unregister when the module shuts down */
	TSet< FName > RegisteredClassNames;
	TSet< FName > RegisteredPropertyTypes;
	TArray<FName> RegisteredComponentClassNames;
};

IMPLEMENT_MODULE(FGridEditor, GridEditor)

void FGridEditor::StartupModule()
{
	RegisterComponentVisualizer();

	FGridEditorCommands::Register();

	FEditorModeRegistry::Get().RegisterMode<FEdModeGridEditor>(EM_GridEditor, NSLOCTEXT("EditorModes", "GridEditorMode", "GridEditor"), FSlateIcon(), true, 1000);

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	RegisterCustomClassLayout("SquareGridSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FSquareSettingsDetails::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FGridEditor::ShutdownModule()
{
	UnregisterComponentVisualizer();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		// Unregister all classes customized by name
		for (auto It = RegisteredClassNames.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomClassLayout(*It);
			}
		}

		// Unregister all structures
		for (auto It = RegisteredPropertyTypes.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomPropertyTypeLayout(*It);
			}
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}

	FEditorModeRegistry::Get().UnregisterMode(EM_GridEditor);
}

void FGridEditor::RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate)
{
	check(ClassName != NAME_None);

	RegisteredClassNames.Add(ClassName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomClassLayout(ClassName, DetailLayoutDelegate);
}

void FGridEditor::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
	check(PropertyTypeName != NAME_None);

	RegisteredPropertyTypes.Add(PropertyTypeName);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

void FGridEditor::RegisterComponentVisualizer()
{
	RegisterComponentVisualizer(UGridSensingComponent::StaticClass()->GetFName(), MakeShareable(new FGridSensingComponentVisualizer));
}

void FGridEditor::RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer)
{
	if (GUnrealEd != nullptr)
	{
		GUnrealEd->RegisterComponentVisualizer(ComponentClassName, Visualizer);
	}

	RegisteredComponentClassNames.Add(ComponentClassName);

	if (Visualizer.IsValid())
	{
		Visualizer->OnRegister();
	}
}

void FGridEditor::UnregisterComponentVisualizer()
{
	if (GUnrealEd != NULL)
	{
		for (FName ClassName : RegisteredComponentClassNames)
		{
			GUnrealEd->UnregisterComponentVisualizer(ClassName);
		}
	}
}

#undef LOCTEXT_NAMESPACE
