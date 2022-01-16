// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "OdysseyStyleSet.h"
#include "EditorModeRegistry.h"
#include "EditorModes.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

#include "PropertyEditorModule.h"

#include "OdysseyViewportDrawingEditorSettings.h"
#include "OdysseyViewportDrawingEditorCommands.h"

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

IMPLEMENT_MODULE(FOdysseyViewportDrawingEditorModule, OdysseyViewportDrawingEditor );

void
FOdysseyViewportDrawingEditorModule::StartupModule()
{
	RegisterEditorMode();

	RegisterCommands();

    RegisterShaders();

	RegisterPropertyModuleCustomizations();
}

void
FOdysseyViewportDrawingEditorModule::ShutdownModule()
{
	UnregisterEditorMode();

	UnregisterCommands();

    UnregisterShaders();

	UnregisterPropertyModuleCustomizations();
}

void
FOdysseyViewportDrawingEditorModule::RegisterEditorMode()
{
	FEditorModeRegistry::Get().RegisterMode<FOdysseyViewportDrawingEditorEdMode>(
		FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId,
		NSLOCTEXT("OdysseyPaintInViewportMode", "OdysseyViewportPaint_ModeName", "Iliad"),
		FSlateIcon(FOdysseyStyle::GetStyleSetName(), "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40", "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16"),
		true, 200 );
}

void
FOdysseyViewportDrawingEditorModule::UnregisterEditorMode()
{
	FEditorModeRegistry::Get().UnregisterMode(FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId);
}

void
FOdysseyViewportDrawingEditorModule::RegisterCommands()
{
    FOdysseyViewportDrawingEditorCommands::Register();
}

void
FOdysseyViewportDrawingEditorModule::UnregisterCommands()
{
    FOdysseyViewportDrawingEditorCommands::Unregister();
}

void
FOdysseyViewportDrawingEditorModule::RegisterShaders()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Iliad"))->GetBaseDir(),TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/Iliad"),PluginShaderDir);
}

void
FOdysseyViewportDrawingEditorModule::UnregisterShaders()
{
	//No method available to unregister Shaders directories
}

void
FOdysseyViewportDrawingEditorModule::RegisterPropertyModuleCustomizations()
{
	/** Register detail/property customization */
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FModuleManager::Get().LoadModule("MeshPaint");
}

void
FOdysseyViewportDrawingEditorModule::UnregisterPropertyModuleCustomizations()
{
	/** De-register detail/property customization */
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomClassLayout("OdysseyViewportDrawingEditorSettings");
		PropertyModule->UnregisterCustomPropertyTypeLayout("OdysseyViewportDrawingEditorTexturePaintSettings");
	}
}
