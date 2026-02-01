// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "ActorFactories/ActorFactory.h"
#include "EditorModeRegistry.h"
#include "Interfaces/IPluginManager.h"

#include "OdysseyStyle.h"

#include "ArianeEditorViewportEdMode.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

/*-----------------------------------------------------------------------------
   FArianeEditorModule
-----------------------------------------------------------------------------*/

void
FArianeEditorModule::OpenStandaloneEditorForAsset( UObject* iAsset )
{
    if (!iAsset)
        return;

/* Gary
    TSharedRef<FArianeEditorStandaloneToolkit> toolkit = MakeShared<FArianeEditorStandaloneToolkit>(iAsset);
    toolkit->Open();
*/
}

/* Gary
void
FArianeEditorModule::AddOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.AddUnique(iEditor);
}


void
FArianeEditorModule::RemoveOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.Remove(iEditor);
}

FOdysseyPainterEditor*
FArianeEditorModule::GetOpenedEditorForAsset(UObject* iObject)
{
    for (FOdysseyPainterEditor* editor : mOpenedEditors)
    {
        UObject* editedObject = editor->GetEditedObject();
        if (editedObject == iObject)
            return editor;

        TArray<UObject*> additionalEditedObjects = editor->GetAdditionalEditedObjects();
        if (additionalEditedObjects.Contains(iObject))
            return editor;
    }

    return nullptr;
}
*/

void
FArianeEditorModule::StartupModule()
{
    RegisterEditorMode();

    //UToolMenus::RegisterStartupCallback( FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FArianeEditorModule::RegisterMenus ) );

/* Gary
    RegisterBrushOverrides(); //First thing to do, as it modifies the Brush CDO
    RegisterSettings();
    RegisterCommands();
    RegisterLevelEditorLayoutExtensions();
    RegisterDetailCustomizations();
    RegisterThumbnailRenderers();
    RegisterPropertyModuleCustomizations();

    FOdysseyVectorBrushCustomization::Register();
    FOdysseyVectorObjectViewPaletteCustomization::Register();
*/
}

void
FArianeEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback( this );

    UnregisterEditorMode();

/* Gary
    UnregisterBrushOverrides();
    UnregisterSettings();
    UnregisterCommands();
    UnregisterLevelEditorLayoutExtensions();
    UnregisterDetailCustomization();
    UnregisterThumbnailRenderers();
    UnregisterPropertyModuleCustomizations();

    FOdysseyVectorBrushCustomization::Unregister();
    FOdysseyVectorObjectViewPaletteCustomization::Unregister();
*/
}

void
FArianeEditorModule::RegisterMenus()
{
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu");
    FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextMoveActions");

}

void
FArianeEditorModule::RegisterEditorMode()
{
    FEditorModeRegistry::Get().RegisterMode<FArianeEditorViewportEdMode>(
        FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId,
        LOCTEXT("ariane-editor-mode.name", "Ariane"),
        FSlateIcon(FOdysseyStyle::GetStyleSetName(), "ArianeViewportDrawingEditMode.ArianeViewportDrawingIcon40", "ArianeViewportDrawingEditMode.ArianeViewportDrawingIcon16"),
        true, 200 );
}

void
FArianeEditorModule::UnregisterEditorMode()
{
    FEditorModeRegistry::Get().UnregisterMode(FArianeEditorViewportEdMode::EM_ArianeEditorViewportEdModeId);
}

void
FArianeEditorModule::RegisterPropertyModuleCustomizations()
{
    /** Register detail/property customization */
/* Gary
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FModuleManager::Get().LoadModule("MeshPaint");
*/
}

void
FArianeEditorModule::UnregisterPropertyModuleCustomizations()
{
/* Gary
    // De-register detail/property customization
    FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
    if (PropertyModule)
    {
        PropertyModule->UnregisterCustomClassLayout("OdysseyViewportDrawingEditorSettings");
        PropertyModule->UnregisterCustomPropertyTypeLayout("OdysseyViewportDrawingEditorTexturePaintSettings");
    }
*/
}

void
FArianeEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

/* Gary
    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditor"
        , LOCTEXT( "user-settings.painter-editor.name", "Odyssey Painter Editor" )
        , LOCTEXT( "user-settings.painter-editor.tooltip", "Configure the look and feel of the Odyssey Editor." )
        , GetMutableDefault<UOdysseyPainterEditorSettings>() );

    settingsModule->RegisterSettings( "Project", "Plugins", "OdysseyAnimationEditor"
        , LOCTEXT( "project-settings.2d-animation-editor.name", "2D Animation Editor" )
        , LOCTEXT( "project-settings.2d-animation-editor.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyPainterEditorAnimationProjectSettings>() );

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditorAnimationUserSettings"
        , LOCTEXT( "user-settings.2d-animation-editor.name", "2D Animation Editor" )
        , LOCTEXT( "user-settings.2d-animation-editor.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyPainterEditorAnimationUserSettings>() );
*/
}

void
FArianeEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

/* Gary
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditorAnimationUserSettings" );
*/
}

void
FArianeEditorModule::RegisterCommands()
{
/* Gary
    FOdysseyPainterEditorCommands::Register();
    FOdysseyPainterEditorAnimationCommands::Register();
    FOdysseyPainterEditorFlipbookCommands::Register();
    FOdysseyViewportDrawingEditorCommands::Register();
*/
}

void
FArianeEditorModule::UnregisterCommands()
{
/* Gary
    FOdysseyPainterEditorCommands::Unregister();
    FOdysseyPainterEditorAnimationCommands::Unregister();
    FOdysseyPainterEditorFlipbookCommands::Unregister();
    FOdysseyViewportDrawingEditorCommands::Unregister();
*/
}

void
FArianeEditorModule::RegisterThumbnailRenderers()
{
/* Gary
    UThumbnailManager::Get().RegisterCustomRenderer(UOdysseyAnimationCell::StaticClass(), UOdysseyAnimationCellThumbnailRenderer::StaticClass());
*/
}

void
FArianeEditorModule::UnregisterThumbnailRenderers()
{
    //UThumbnailManager::Get().UnregisterCustomRenderer(UOdysseyAnimationCellImageRaster::StaticClass());
}

void
FArianeEditorModule::RegisterBrushOverrides()
{
/* Gary
    FOdysseyBrushOverride::Register(UOdysseyBrushOptionsOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyBlendParametersOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyFreehandShapeOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyPainterEditorRasterDrawingToolOverrides::StaticClass());
*/
}

void
FArianeEditorModule::UnregisterBrushOverrides()
{

}

void
FArianeEditorModule::RegisterLevelEditorLayoutExtensions()
{
/* Gary
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyPainterEditorLevelEditorLayout::ExtendLevelEditorLayout);
*/
}

void
FArianeEditorModule::UnregisterLevelEditorLayoutExtensions()
{
/* Gary
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
*/
}

void
FArianeEditorModule::RegisterDetailCustomizations()
{
/* Gary
    FOdysseyShapes::RegisterDetailCustomization();
    UOdysseyPainterEditorRasterLiquifyTool::RegisterDetailCustomization();

    FOdysseyPainterEditorAnimationFlipSystem::RegisterDetailCustomization();
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyPainterEditorAnimationOutOfPegsTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyPainterEditorAnimationOutOfPegsToolDetails::MakeInstance));
*/
}

void
FArianeEditorModule::UnregisterDetailCustomization()
{
/* Gary
    UOdysseyPainterEditorRasterLiquifyTool::UnregisterDetailCustomization();
    FOdysseyShapes::UnregisterDetailCustomization();
    FOdysseyPainterEditorAnimationFlipSystem::UnregisterDetailCustomization();
*/
}

IMPLEMENT_MODULE( FArianeEditorModule, ArianeEditor );

#undef LOCTEXT_NAMESPACE
