// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#include "OdysseyBrushEditorModule.h"
#include "Editor.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_OdysseyBrush.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushThumbnailRenderer.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "OdysseyBrush_ContentBrowserExtensions.h"

#define LOCTEXT_NAMESPACE "BrushEditor"

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushEditorModule
void FOdysseyBrushEditorModule::StartupModule()
{
    //check(GEditor);

    /////////////////
    // Register the asset type
    IAssetTools&  AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >( "AssetTools" ).Get();

    // Basic Category
    EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory( FName( TEXT( "Odyssey" ) ), LOCTEXT( "asset-category.name", "Odyssey" ) );

    OdysseyBrushAssetTypeActions = MakeShareable( new  FAssetTypeActions_OdysseyBrush( OdysseyBrushAssetCategoryBit ) );
    AssetTools.RegisterAssetTypeActions( OdysseyBrushAssetTypeActions.ToSharedRef() );

    /////////////////

    if( !IsRunningCommandlet() )
    {
        FOdysseyBrushContentBrowserExtensions::InstallHooks();
    }
}

void FOdysseyBrushEditorModule::ShutdownModule()
{
    /////////////////
    if( !UObjectInitialized() )
    {
        return;
    }

    // Only unregister if the asset tools module is loaded.  We don't want to forcibly load it during shutdown phase.
    check( OdysseyBrushAssetTypeActions.IsValid() );
    if( FModuleManager::Get().IsModuleLoaded( "AssetTools" ) )
        FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" ).Get().UnregisterAssetTypeActions( OdysseyBrushAssetTypeActions.ToSharedRef() );
    OdysseyBrushAssetTypeActions.Reset();

    // Unregister the thumbnail renderers
    UThumbnailManager::Get().UnregisterCustomRenderer( UOdysseyBrush::StaticClass() );
    /////////////////
}


IMPLEMENT_MODULE( FOdysseyBrushEditorModule, OdysseyBrushEditor );


#undef LOCTEXT_NAMESPACE
