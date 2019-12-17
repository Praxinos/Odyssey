// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000


#include "OdysseyBrushEditorModule.h"
#include "Editor.h"
#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_OdysseyBrush.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushThumbnailRenderer.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorModule"

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushEditorModule
void FOdysseyBrushEditorModule::StartupModule()
{
    check(GEditor);

    /////////////////
    // Register the asset type
    IAssetTools&  AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >( "AssetTools" ).Get();

    // Sub Menu Advanced Category
    //EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("OdysseyBrush")), LOCTEXT("OdysseyBrushAssetCategory", "OdysseyBrush"));
    // Basic Category
    EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = EAssetTypeCategories::Type::Basic;

    OdysseyBrushAssetTypeActions = MakeShareable( new  FAssetTypeActions_OdysseyBrush( OdysseyBrushAssetCategoryBit ) );
    AssetTools.RegisterAssetTypeActions( OdysseyBrushAssetTypeActions.ToSharedRef() );

    // Register the thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer( UOdysseyBrush::StaticClass(), UOdysseyBrushThumbnailRenderer::StaticClass() );
    /////////////////

    // Register internal SCS editor customizations
    //RegisterSCSEditorCustomization("InstancedStaticMeshComponent", FOdysseySCSEditorCustomizationBuilder::CreateStatic(&FInstancedStaticMeshSCSEditorCustomization::MakeInstance));
    //RegisterSCSEditorCustomization("HierarchicalInstancedStaticMeshComponent", FOdysseySCSEditorCustomizationBuilder::CreateStatic(&FInstancedStaticMeshSCSEditorCustomization::MakeInstance));
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
