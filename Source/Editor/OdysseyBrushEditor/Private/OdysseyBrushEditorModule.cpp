// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022


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
    EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory( FName( TEXT( "ILIAD" ) ), LOCTEXT( "asset-category.name", "ILIAD" ) );

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
