// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBrushEditorModule.h"

#include "AssetToolsModule.h"
#include "Editor.h"
#include "Modules/ModuleManager.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#include "AssetTypeActions_OdysseyBrush.h"
#include "OdysseyBrush_ContentBrowserExtensions.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushThumbnailRenderer.h"
#include "OdysseyTelemetryModule.h"
#include "OdysseyTelemetry.h"

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

    RegisterTelemetry();
}

void FOdysseyBrushEditorModule::ShutdownModule()
{
    UnregisterTelemetry();

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


void
FOdysseyBrushEditorModule::RegisterTelemetry()
{
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UOdysseyBrush::StaticClass() );

    // GEditor and/or Subsystem may not be available directly here (in a StartupModule),
    // so delegate it once the engine is fully loaded
    FCoreDelegates::GetOnPostEngineInit().AddLambda( [this]()
                                                     {
                                                         UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
                                                         AssetEditorSubsystem->OnAssetEditorOpened().AddRaw( this, &FOdysseyBrushEditorModule::OnAssetEditorOpened );
                                                         AssetEditorSubsystem->OnAssetClosedInEditor().AddRaw( this, &FOdysseyBrushEditorModule::OnAssetEditorClosed );
                                                     } );
}

void
FOdysseyBrushEditorModule::UnregisterTelemetry()
{
    if( GEditor )
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        AssetEditorSubsystem->OnAssetEditorOpened().RemoveAll( this );
        AssetEditorSubsystem->OnAssetClosedInEditor().RemoveAll( this );
    }

    FCoreDelegates::GetOnPostEngineInit().RemoveAll( this );

    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UOdysseyBrush::StaticClass() );
}

void
FOdysseyBrushEditorModule::OnAssetEditorOpened( UObject* iObject )
{
    UOdysseyBrush* brush = Cast<UOdysseyBrush>( iObject );
    if( !brush )
        return;

    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    IAssetEditorInstance* Editor = AssetEditorSubsystem->FindEditorForAsset( iObject, false );

    SessionInfoByObject.Add( brush, { FDateTime::UtcNow(), Editor->GetEditorName() } );
}

void
FOdysseyBrushEditorModule::OnAssetEditorClosed( UObject* iObject, IAssetEditorInstance* iAssetEditorInstance )
{
    UOdysseyBrush* brush = Cast<UOdysseyBrush>( iObject );
    if( !brush )
        return;

    FSessionInfo SessionInfo = SessionInfoByObject.FindAndRemoveChecked( brush );

    {
        using FAssetEditionFields = FAssetEdition_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( FAssetEditionFields::EditorName_KeyName_AsString, FString::Printf( TEXT( "%s:Brush" ), *SessionInfo.EditorName.ToString() ) );
        Attributes.Emplace( FAssetEditionFields::SessionDuration_KeyName_AsDouble, ( FDateTime::UtcNow() - SessionInfo.SessionStartTime ).GetTotalSeconds() );

        FOdysseyTelemetry::Get().RecordEvent( FAssetEditionFields::KeyName, Attributes );
    }
}

IMPLEMENT_MODULE( FOdysseyBrushEditorModule, OdysseyBrushEditor );


#undef LOCTEXT_NAMESPACE
