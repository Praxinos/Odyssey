// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OdysseyViewportDrawingEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "OdysseyStyleSet.h"
#include "EditorModeRegistry.h"
#include "EditorModes.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

#include "PropertyEditorModule.h"

#include "OdysseyViewportDrawingEditorSettings.h"

#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FOdysseyViewportDrawingEditorModule, OdysseyViewportDrawingEditor );

void FOdysseyViewportDrawingEditorModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FOdysseyViewportDrawingEditorEdMode>(
		FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId,
		NSLOCTEXT("OdysseyPaintInViewportMode", "OdysseyViewportPaint_ModeName", "Paint in Viewport"),
		FSlateIcon(FOdysseyStyle::GetStyleSetName(), "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40", "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16"),
		true, 200 );

	/** Register detail/property customization */
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FModuleManager::Get().LoadModule("MeshPaint");
}

void FOdysseyViewportDrawingEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId);

	/** De-register detail/property customization */
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomClassLayout("OdysseyViewportDrawingEditorSettings");
		PropertyModule->UnregisterCustomPropertyTypeLayout("OdysseyViewportDrawingEditorTexturePaintSettings");
	}
}
