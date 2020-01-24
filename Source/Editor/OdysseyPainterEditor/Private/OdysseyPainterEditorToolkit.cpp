// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyPainterEditorToolkit.h"

#include "AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "Dialogs/Dialogs.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "IContentBrowserSingleton.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Slate/SceneViewport.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"

#include "IOdysseyPainterEditorModule.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyAboutScreen.h"
#include "OdysseyBlock.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyImageLayer.h"
#include "OdysseyPainterEditorViewportClient.h"
#include "OdysseyStyleSet.h"
#include "SOdysseySurfaceViewport.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/*static*/const FName FOdysseyPainterEditorToolkit::smViewportTabId( TEXT( "OdysseyPainterEditor_Viewport" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smBrushSelectorTabId( TEXT( "OdysseyPainterEditor_BrushSelector" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smMeshSelectorTabId( TEXT( "OdysseyPainterEditor_MeshSelector" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smBrushExposedParametersTabId( TEXT( "OdysseyPainterEditor_BrushExposedParameters" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smColorSelectorTabId( TEXT( "OdysseyPainterEditor_ColorSelector" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smColorSlidersTabId( TEXT( "OdysseyPainterEditor_ColorSliders" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smLayerStackTabId( TEXT( "OdysseyPainterEditor_LayerStack" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smBotBarTabId( TEXT( "OdysseyPainterEditor_BotBar" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smTopBarTabId( TEXT( "OdysseyPainterEditor_TopBar" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smStrokeOptionsTabId( TEXT( "OdysseyPainterEditor_StrokeOptions" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smNotesTabId( TEXT( "OdysseyPainterEditor_Notes" ) );
// /*static*/const FName FOdysseyPainterEditorToolkit::smUndoHistoryTabId( TEXT( "OdysseyPainterEditor_UndoHistory" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smPerformanceOptionsTabId( TEXT( "OdysseyPainterEditor_PerformanceOptions" ) );
/*static*/const FName FOdysseyPainterEditorToolkit::smToolsTabId( TEXT( "OdysseyPainterEditor_Tools" ) );

const FName OdysseyPainterEditorAppIdentifier = FName( TEXT( "OdysseyPainterEditorApp" ) );

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolkit::~FOdysseyPainterEditorToolkit()
{
    //GEditor->UnregisterForUndo(this);
    if( mBrushInstance )
    {
        mBrushInstance->CleansePool( ECacheLevel::kSuper );
        mBrushInstance->CleansePools();
        mBrushInstance->RemoveFromRoot();
        mBrushInstance = NULL;
    }
    if( mDisplaySurface )
        delete mDisplaySurface;
}

FOdysseyPainterEditorToolkit::FOdysseyPainterEditorToolkit()
    : mScopedTransaction( NULL )
    , mUndoHistory()
    , mIsManipulationDirtiedSomething( false )
    , mIsEditorMarkedAsClosed( false )
    , mTexture( NULL )
    , mDisplaySurface( NULL )
    , mTextureContentsBackup( NULL )
    , mTextureMipGenBackup()
    , mTextureCompressionBackup()
    , mTextureGroupBackup()
    , mPaintEngine( &mUndoHistory )
    , mLayerStack()
    , mBrush( NULL )
    , mBrushInstance( NULL )
    , mLiveUpdateInfo()
    , mIsTextureDirty( false )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorToolkit::InitOdysseyPainterEditor( const EToolkitMode::Type iMode,
                                                        const TSharedPtr< class IToolkitHost >& iInitToolkitHost,
                                                        UTexture2D* iTexture )
{
    // Setup Texture
    mTexture = iTexture;
    mTextureMipGenBackup = mTexture->MipGenSettings;
    mTextureCompressionBackup = mTexture->CompressionSettings;
    mTextureGroupBackup = mTexture->LODGroup;

    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;

    mTexture->UpdateResource();
    mTextureContentsBackup = NewOdysseyBlockFromUTextureData( mTexture );

    // Setup Layers
    mLayerStack.InitFromData( mTextureContentsBackup );
    mLayerStack.ComputeResultBlock();

    // Setup Paint Engine
    mPaintEngine.SetTextureSourceFormat( mTextureContentsBackup->GetUE4TextureSourceFormat() );
    mPaintEngine.SetLayerStack( &mLayerStack );
    mPaintEngine.SetBrushInstance( NULL );
    mPaintEngine.SetColor( ::ULIS::CColor() );
    mPaintEngine.SetSizeModifier( 20.f );

    // Setup Surface
    mDisplaySurface = new FOdysseySurface( mLayerStack.GetResultBlock() );
    mLiveUpdateInfo.main = mDisplaySurface->Texture();
    mLiveUpdateInfo.live = mTexture;
    mLiveUpdateInfo.enabled = false;
    mDisplaySurface->Block()->GetIBlock()->SetInvalidateCB( &InvalidateLiveSurfaceCallback, static_cast<void*>( &mLiveUpdateInfo ) );

    mDisplaySurface->Invalidate();

    // Support undo/redo
    //mDisplaySurface->Texture()->SetFlags( RF_Transactional );
    //GEditor->RegisterForUndo( this );

    // Register our commands. This will only register them if not previously registered
    FOdysseyPainterEditorCommands::Register();

    // Build commands
    BindCommands();

    // Create tabs contents
    CreateMeshSelectorTab();
    CreateViewportTab();
    CreateBrushSelectorTab();
    CreateBrushExposedParametersTab();
    CreateLayerStackTab(); //mLayerStack have to be initialized at this point
    CreateColorSelectorTab();
    CreateColorSlidersTab();
    CreateTopTab();
    CreateStrokeOptionsTab();
    CreatePerformanceOptionsTab();
    //CreateUndoHistoryTab();


    // Setup Properties with callbacks
    mColorSelectorTab->SetColor( ::ULIS::CColor( 0, 0, 0 ) );

    mStrokeOptionsTab->SetStrokeStep( 20 );
    mStrokeOptionsTab->SetStrokeAdaptative( true );
    mStrokeOptionsTab->SetStrokePaintOnTick( false );
    mStrokeOptionsTab->SetInterpolationType( (int32)EOdysseyInterpolationType::kBezier );
    mStrokeOptionsTab->SetSmoothingMethod( (int32)EOdysseySmoothingMethod::kAverage );
    mStrokeOptionsTab->SetSmoothingStrength( 10 );
    mStrokeOptionsTab->SetSmoothingEnabled( true );
    mStrokeOptionsTab->SetSmoothingRealTime( true );
    mStrokeOptionsTab->SetSmoothingCatchUp( true );

    mPerformanceOptionsTab->SetPerformanceOptionLiveUpdate( true );

    mTopTab->SetSize( 20 );
    mTopTab->SetOpacity( 100 );
    mTopTab->SetFlow( 100 );

    // Build Layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout( "Standalone_OdysseyPainterEditor_Layout" )
    ->AddArea
    (
        FTabManager::NewPrimaryArea()
        ->SetOrientation( Orient_Horizontal )
        ->Split
        (
            FTabManager::NewSplitter()
            ->SetOrientation( Orient_Vertical )
            ->SetSizeCoefficient( 1.f )
            // TopMost Part
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetSizeCoefficient( 1.f )
                ->SetOrientation( Orient_Horizontal )
                // Left Bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation( Orient_Vertical )
                    ->SetSizeCoefficient( 0.2f )
                    // Brush Selector
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smBrushSelectorTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.1f )
                    )
                    // Brush Preview
                    ->Split
                    (
                        FTabManager::NewStack()
                        /*->AddTab( FName( "BrushPreview" ), ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.1f )*/
                        // Brush Params
                        ->AddTab( smBrushExposedParametersTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.1f )
                    )
                    // Brush Params + Stroke Options
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )
                        // Stroke Options
                        ->AddTab( smStrokeOptionsTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )
                        // Performance Options
                        ->AddTab( smPerformanceOptionsTabId, ETabState::ClosedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )
                        // Mesh Selector
                        ->AddTab( smMeshSelectorTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )
                    )
                    // Tools
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smToolsTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.2f )
                    )
                    // Navigator
                    /*->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( FName( "Navigator" ), ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )

                        // Notes
                        ->AddTab( smNotesTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.3f )
                    )*/
                )
                // Middle bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation( Orient_Vertical )
                    // Top Bar
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smTopBarTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( true )
                        ->SetSizeCoefficient( 0.1f )
                    )
                    // Viewport
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smViewportTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.9f )
                    )
                )
                // Right bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetSizeCoefficient( 0.16f )
                    ->SetOrientation( Orient_Vertical )
                    // ColorSelector
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smColorSelectorTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.2f )
                    )
                    // ColorSliders
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( smColorSlidersTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.2f )
                    )
                    // LayerStack + Notes
                    ->Split
                    (
                        FTabManager::NewStack()
                        // Undo History
                        //->AddTab( UndoHistoryTabId, ETabState::ClosedTab )
                        //->SetHideTabWell( false )
                        //->SetSizeCoefficient( 0.6f )
                        // Layer Stack
                        ->AddTab( smLayerStackTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient( 0.6f )
                    )
                )
            )
            // Bottom Part
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab( FName( smBotBarTabId ), ETabState::OpenedTab )
                ->SetHideTabWell( true )
            )
        )
    );

    IOdysseyPainterEditorModule* odysseyPainterEditorModule = &FModuleManager::LoadModuleChecked<IOdysseyPainterEditorModule>( "OdysseyPainterEditor" );

    FAssetEditorToolkit::InitAssetEditor( iMode, iInitToolkitHost, OdysseyPainterEditorAppIdentifier, StandaloneDefaultLayout, true, false, mTexture );

    InitializeExtenders();

    RegenerateMenusAndToolbars();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
void
FOdysseyPainterEditorToolkit::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    WorkspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory( LOCTEXT( "WorkspaceMenu_OdysseyPainterEditor", "Odyssey Painter Editor" ) );
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
    FAssetEditorToolkit::RegisterTabSpawners( iTabManager );

    //The viewport: the central area where we can draw on images
    iTabManager->RegisterTabSpawner( smViewportTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnViewport ) )
        .SetDisplayName( LOCTEXT( "ViewportTab", "Viewport" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ) );

    // BrushSelector
    iTabManager->RegisterTabSpawner( smBrushSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushSelector ) )
        .SetDisplayName( LOCTEXT( "BrushSelectorTab", "Brush Selector" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ) );

    // MeshSelector
    iTabManager->RegisterTabSpawner( smMeshSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnMeshSelector ) )
        .SetDisplayName( LOCTEXT( "MeshSelectorTab", "Mesh Selector" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ) );

    // BrushExposedParameters
    iTabManager->RegisterTabSpawner( smBrushExposedParametersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushExposedParameters ) )
        .SetDisplayName( LOCTEXT( "BrushExposedParametersTab", "Brush Exposed Parameters" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ) );

    // ColorSelector
    iTabManager->RegisterTabSpawner( smColorSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSelector ) )
        .SetDisplayName( LOCTEXT( "ColorSelectorTab", "ColorSelector" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ) );

    // ColorSliders
    iTabManager->RegisterTabSpawner( smColorSlidersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSliders ) )
        .SetDisplayName( LOCTEXT( "ColorSlidersTab", "ColorSliders" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ) );

    // LayerStack
    iTabManager->RegisterTabSpawner( smLayerStackTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnLayerStack ) )
        .SetDisplayName( LOCTEXT( "LayerStackTab", "LayerStack" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ) );

    // BotBar
    iTabManager->RegisterTabSpawner( smBotBarTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBotBar ) )
        .SetDisplayName( LOCTEXT( "BotBarTab", "BotBar" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BotBar16" ) );

    // TopBar
    iTabManager->RegisterTabSpawner( smTopBarTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTopBar ) )
        .SetDisplayName( LOCTEXT( "TopBarTab", "TopBar" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.TopBar16" ) );

    // StrokeOptions
    iTabManager->RegisterTabSpawner( smStrokeOptionsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnStrokeOptions ) )
        .SetDisplayName( LOCTEXT( "StrokeOptionsTab", "Stroke Options" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ) );

    // PerformanceOptions
    iTabManager->RegisterTabSpawner( smPerformanceOptionsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnPerformanceOptions ) )
        .SetDisplayName( LOCTEXT( "PerformanceOptionsTab", "Performance Options" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.PerformanceTools16" ) );

    // Notes
    iTabManager->RegisterTabSpawner( smNotesTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnNotes ) )
        .SetDisplayName( LOCTEXT( "NotesTab", "Notes" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Notes16" ) );

    // Undo History
    /*iTabManager->RegisterTabSpawner( UndoHistoryTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnUndoHistory ) )
        .SetDisplayName( LOCTEXT( "UndoHistoryTab", "UndoHistory" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.UndoHistory16" ) );*/

        // Tools
    iTabManager->RegisterTabSpawner( smToolsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTools ) )
        .SetDisplayName( LOCTEXT( "ToolsTab", "Tools" ) )
        .SetGroup( WorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) );
}

void
FOdysseyPainterEditorToolkit::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    FAssetEditorToolkit::UnregisterTabSpawners( iTabManager );
    iTabManager->UnregisterTabSpawner( smViewportTabId );
    iTabManager->UnregisterTabSpawner( smBrushSelectorTabId );
    iTabManager->UnregisterTabSpawner( smMeshSelectorTabId );
    iTabManager->UnregisterTabSpawner( smBrushExposedParametersTabId );
    iTabManager->UnregisterTabSpawner( smColorSelectorTabId );
    iTabManager->UnregisterTabSpawner( smColorSlidersTabId );
    iTabManager->UnregisterTabSpawner( smLayerStackTabId );
    iTabManager->UnregisterTabSpawner( smBotBarTabId );
    iTabManager->UnregisterTabSpawner( smTopBarTabId );
    iTabManager->UnregisterTabSpawner( smStrokeOptionsTabId );
    iTabManager->UnregisterTabSpawner( smPerformanceOptionsTabId );
    iTabManager->UnregisterTabSpawner( smNotesTabId );
    //iTabManager->UnregisterTabSpawner( smUndoHistoryTabId );
    iTabManager->UnregisterTabSpawner( smToolsTabId );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
FText
FOdysseyPainterEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Painter Editor" );
}

FName
FOdysseyPainterEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyPainterEditor" );
}

FLinearColor
FOdysseyPainterEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

FString
FOdysseyPainterEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Texture" ).ToString();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FEditorUndoClient interface
void
FOdysseyPainterEditorToolkit::PostUndo( bool iSuccess )
{
}

void
FOdysseyPainterEditorToolkit::PostRedo( bool iSuccess )
{
    PostUndo( iSuccess );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override
void
FOdysseyPainterEditorToolkit::SaveAsset_Execute()
{
    // Commit changes permanently
    //mDisplaySurface->CommitBlockChangesIntoTextureBulk();
    // Reload backup
    CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    ::ULIS::FMakeContext::CopyBlockInto( mDisplaySurface->Block()->GetIBlock(), mTextureContentsBackup->GetIBlock() );
    InvalidateTextureFromData( mDisplaySurface->Block(), mTexture );
    // Invalidate all
    mDisplaySurface->Invalidate();

    FAssetEditorToolkit::SaveAsset_Execute();
}

void
FOdysseyPainterEditorToolkit::SaveAssetAs_Execute()
{
    /*
    CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    //::ULIS::FMakeContext::CopyBlockInto( mDisplaySurface->Block()->GetIBlock(), mTextureContentsBackup->GetIBlock() );
    InvalidateTextureFromData( mDisplaySurface->Block(), mTexture );
    // Invalidate all
    mDisplaySurface->Invalidate();

    FAssetEditorToolkit::SaveAssetAs_Execute();

    CopyBlockDataIntoUTexture( mTextureContentsBackup, mTexture );

    InvalidateTextureFromData( mTextureContentsBackup, mTexture );
    InvalidateSurfaceFromData( mTextureContentsBackup, mDisplaySurface );
     */
}

bool
FOdysseyPainterEditorToolkit::OnRequestClose()
{
    if( !mIsEditorMarkedAsClosed )
    {
        EAppReturnType::Type returnType = OpenMsgDlgInt( EAppMsgType::YesNoCancel
                                                        , FText::Format( LOCTEXT( "Save Texture Prompt"
                                                                                , "Save the texture {0} before exiting ?"
                                                                                  "\n"
                                                                                  "\n"
                                                                                  "Warning: when closing the texture, layers will be merged, "
                                                                                  "export them first if you want to keep them ! (File/Export Layers)" )
                                                                                , FText::FromString( mTexture->GetFName().ToString() ) )
                                                        , LOCTEXT( "Save Texture Title", "Save Texture" ) );

        if( returnType == EAppReturnType::Cancel )
            return false;

        if( returnType == EAppReturnType::Yes )
            SaveAsset_Execute();

        // Invalidate All from backup data
        // If saved, no change
        // If unsaved, revert display to last saved data
        InvalidateTextureFromData( mTextureContentsBackup, mTexture );
        InvalidateSurfaceFromData( mTextureContentsBackup, mDisplaySurface );
    }
    mTexture->LODGroup = mTextureGroupBackup;
    mIsEditorMarkedAsClosed = true;
    mLayerStack.mDrawingUndo->Clear();
    return true;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building
void
FOdysseyPainterEditorToolkit::BindCommands()
{
    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::OnImportTexturesAsLayers ),
        FCanExecuteAction() );

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::OnExportLayersAsTextures ),
        FCanExecuteAction() );

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().AboutIliad,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::OnAboutIliad ),
        FCanExecuteAction() );
    
    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().Undo,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::UndoIliad ),
        FCanExecuteAction() );
    
    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().Redo,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::RedoIliad ),
        FCanExecuteAction() );

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::OnVisitPraxinosWebsite ),
        FCanExecuteAction() );

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosForums,
        FExecuteAction::CreateSP( this, &FOdysseyPainterEditorToolkit::OnVisitPraxinosForums ),
        FCanExecuteAction() );
}

void
FOdysseyPainterEditorToolkit::InitializeExtenders()
{
    // Create the Extender that will add content to the menu
    TSharedPtr<FExtender> MenuExtender = MakeShareable( new FExtender() );
    SetupOdysseyPainterEditorMenu( MenuExtender, *this );
    AddMenuExtender( MenuExtender );

    IOdysseyPainterEditorModule* OdysseyPainterEditorModule = &FModuleManager::LoadModuleChecked<IOdysseyPainterEditorModule>( "OdysseyPainterEditor" );
}

void
FOdysseyPainterEditorToolkit::SetupOdysseyPainterEditorMenu( TSharedPtr< FExtender > ioExtender, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor )
{
    ioExtender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
        iOdysseyPainterEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorToolkit& >( &FOdysseyPainterEditorToolkit::FillImportExportMenu, iOdysseyPainterEditor ) );

    ioExtender->AddMenuExtension(
        "HelpApplication",
        EExtensionHook::After,
        iOdysseyPainterEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorToolkit& >( &FOdysseyPainterEditorToolkit::FillAboutMenu, iOdysseyPainterEditor ) );
}

//static
void
FOdysseyPainterEditorToolkit::FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor )
{
    ioMenuBuilder.BeginSection( "FileOdysseyPainter", LOCTEXT( "OdysseyPainter", "OdysseyPainter" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures );
    }
}

//static
void
FOdysseyPainterEditorToolkit::FillAboutMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor )
{
    ioMenuBuilder.BeginSection( "About", LOCTEXT( "OdysseyPainter", "OdysseyPainter" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().AboutIliad );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosForums );
    }
}

void
FOdysseyPainterEditorToolkit::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ExportLayerDialogTitle", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath( mTexture->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = mTexture->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if( saveObjectPath != "" )
    {
        TArray<TSharedPtr<IOdysseyLayer>>* layers = mLayerStack.GetLayers();

        for( int i = 0; i < layers->Num(); i++ )
        {
            if( !( ( *layers )[i].Get()->GetType() == IOdysseyLayer::eType::kImage ) )
                continue;

            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*> ( ( *layers )[i].Get() );

            FString assetPath = FPaths::GetPath( saveObjectPath ) + "/";
            FString packagePath = ( assetPath + imageLayer->GetName().ToString().Replace( TEXT( " " ), TEXT( "_" ) ) );
            UPackage* package = CreatePackage( nullptr, *packagePath );

            UTexture2D* object = NewObject<UTexture2D>( package, UTexture2D::StaticClass(), FName( *( FPaths::GetBaseFilename( saveObjectPath ) + TEXT( "_" ) + imageLayer->GetName().ToString() ) ), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone );
            object->Source.Init( imageLayer->GetBlock()->Width(), imageLayer->GetBlock()->Height(), 1, 1, TSF_BGRA8 );
            object->PostEditChange();

            CopyBlockDataIntoUTexture( imageLayer->GetBlock(), object );

            object->UpdateResource();

            FAssetRegistryModule::AssetCreated( object );
            object->MarkPackageDirty();

            bool success = UPackage::SavePackage( package, object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *( imageLayer->GetName().ToString() ) );
        }
    }
}

void
FOdysseyPainterEditorToolkit::OnImportTexturesAsLayers()
{
    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures As Layers" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath( mTexture->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>( assetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture );
        mLayerStack.AddLayerFromData( textureBlock, FName( *( openedTexture->GetName() ) ) );
        delete textureBlock;
    }

    mLayerStackTab->RefreshView();
    mLayerStack.ComputeResultBlock();
}

void
FOdysseyPainterEditorToolkit::OnAboutIliad()
{
    const FText aboutWindowTitle = LOCTEXT( "AboutIliad", "About Iliad" );

    TSharedPtr<SWindow> aboutWindow =
        SNew( SWindow )
        .Title( aboutWindowTitle )
        .ClientSize( FVector2D( 600.f, 460.f ) )
        .SupportsMaximize( false )
        .SupportsMinimize( false )
        .SizingRule( ESizingRule::FixedSize )
        [
            SNew( SOdysseyAboutScreen )
        ];

    FSlateApplication::Get().AddModalWindow( aboutWindow.ToSharedRef(), this->GetToolkitHost()->GetParentWidget() );
}

void
FOdysseyPainterEditorToolkit::OnVisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorToolkit::OnVisitPraxinosForums()
{
    FString URL = "https://praxinos.coop/forum";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation
void
FOdysseyPainterEditorToolkit::CreateViewportTab()
{
    mViewportTab = SNew( SOdysseySurfaceViewport );
    mViewportTab->SetSurface( mDisplaySurface );
    mViewportTab->SetViewportClient( MakeShareable( new FOdysseyPainterEditorViewportClient( SharedThis( this ), mViewportTab, mMeshSelectorTab->GetMeshSelectorPtr() ) ) );
}

void
FOdysseyPainterEditorToolkit::CreateBrushSelectorTab()
{
    mBrushSelectorTab = SNew( SOdysseyBrushSelector )
        .OnBrushChanged( this, &FOdysseyPainterEditorToolkit::OnBrushSelected );
}

void
FOdysseyPainterEditorToolkit::CreateMeshSelectorTab()
{
    mMeshSelectorTab = SNew( SOdysseyMeshSelector )
        .OnMeshChanged( this, &FOdysseyPainterEditorToolkit::OnMeshSelected );
}

void
FOdysseyPainterEditorToolkit::CreateBrushExposedParametersTab()
{
    mBrushExposedParametersTab = SNew( SOdysseyBrushExposedParameters )
        .OnParameterChanged( this, &FOdysseyPainterEditorToolkit::HandleBrushParameterChanged );
}

void
FOdysseyPainterEditorToolkit::CreateLayerStackTab()
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData( MakeShareable( &mLayerStack ) );
}

void
FOdysseyPainterEditorToolkit::CreateColorSelectorTab()
{
    mColorSelectorTab = SNew( SOdysseyColorSelector )
        .OnColorChanged( this, &FOdysseyPainterEditorToolkit::HandleSelectorColorChanged );
}

void
FOdysseyPainterEditorToolkit::CreateColorSlidersTab()
{
    mColorSlidersTab = SNew( SOdysseyColorSliders )
        .OnColorChanged( this, &FOdysseyPainterEditorToolkit::HandleSlidersColorChanged );
}

void
FOdysseyPainterEditorToolkit::CreateToolsTab()
{
}

void
FOdysseyPainterEditorToolkit::CreateTopTab()
{
    mTopTab = SNew( SOdysseyPaintModifiers )
        .OnSizeChanged          ( this, &FOdysseyPainterEditorToolkit::HandleSizeModifierChanged )
        .OnOpacityChanged       ( this, &FOdysseyPainterEditorToolkit::HandleOpacityModifierChanged )
        .OnFlowChanged          ( this, &FOdysseyPainterEditorToolkit::HandleFlowModifierChanged )
        .OnBlendingModeChanged  ( this, &FOdysseyPainterEditorToolkit::HandleBlendingModeModifierChanged )
        .OnAlphaModeChanged     ( this, &FOdysseyPainterEditorToolkit::HandleAlphaModeModifierChanged );
}

void
FOdysseyPainterEditorToolkit::CreateStrokeOptionsTab()
{
    mStrokeOptionsTab = SNew( SOdysseyStrokeOptions )
        .OnStrokeStepChanged        ( this, &FOdysseyPainterEditorToolkit::HandleStrokeStepChanged )
        .OnStrokeAdaptativeChanged  ( this, &FOdysseyPainterEditorToolkit::HandleStrokeAdaptativeChanged )
        .OnStrokePaintOnTickChanged ( this, &FOdysseyPainterEditorToolkit::HandleStrokePaintOnTickChanged )
        .OnInterpolationTypeChanged ( this, &FOdysseyPainterEditorToolkit::HandleInterpolationTypeChanged )
        .OnSmoothingMethodChanged   ( this, &FOdysseyPainterEditorToolkit::HandleSmoothingMethodChanged )
        .OnSmoothingStrengthChanged ( this, &FOdysseyPainterEditorToolkit::HandleSmoothingStrengthChanged )
        .OnSmoothingEnabledChanged  ( this, &FOdysseyPainterEditorToolkit::HandleSmoothingEnabledChanged )
        .OnSmoothingRealTimeChanged ( this, &FOdysseyPainterEditorToolkit::HandleSmoothingRealTimeChanged )
        .OnSmoothingCatchUpChanged  ( this, &FOdysseyPainterEditorToolkit::HandleSmoothingCatchUpChanged );
}

void
FOdysseyPainterEditorToolkit::CreatePerformanceOptionsTab()
{
    mPerformanceOptionsTab = SNew( SOdysseyPerformanceOptions )
        .OnLiveUpdateChanged( this, &FOdysseyPainterEditorToolkit::HandlePerformanceLiveUpdateChanged );
}

/*
void
FOdysseyPainterEditorToolkit::CreateUndoHistoryTab()
{
    mUndoHistoryTab = SNew( SOdysseyUndoHistory, &mUndoHistory );
}*/

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- Paint engine driving methods
void
FOdysseyPainterEditorToolkit::OnBrushSelected( UOdysseyBrush* iBrush )
{
    mBrush = iBrush;

    if( mBrushInstance )
    {
        mBrushInstance->RemoveFromRoot();
        mBrushInstance = NULL;
    }

    if( mBrush )
    {
        //@todo: check
        //mBrush->OnChanged().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushChanged );

        mBrush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        mBrushInstance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), mBrush->GeneratedClass );
        mBrushInstance->AddToRoot();

        mPaintEngine.SetBrushInstance( mBrushInstance );
        mBrushExposedParametersTab->Refresh( mBrushInstance );

        FOdysseyBrushPreferencesOverrides& overrides = mBrushInstance->Preferences;
        if( overrides.bOverride_Step )          mStrokeOptionsTab->SetStrokeStep( overrides.Step );
        if( overrides.bOverride_Adaptative )    mStrokeOptionsTab->SetStrokeAdaptative( overrides.SizeAdaptative );
        if( overrides.bOverride_PaintOnTick )   mStrokeOptionsTab->SetStrokePaintOnTick( overrides.PaintOnTick );
        if( overrides.bOverride_Type )          mStrokeOptionsTab->SetInterpolationType( (int32)overrides.Type );
        if( overrides.bOverride_Method )        mStrokeOptionsTab->SetSmoothingMethod( (int32)overrides.Method );
        if( overrides.bOverride_Strength )      mStrokeOptionsTab->SetSmoothingStrength( overrides.Strength );
        if( overrides.bOverride_Enabled )       mStrokeOptionsTab->SetSmoothingEnabled( overrides.Enabled );
        if( overrides.bOverride_RealTime )      mStrokeOptionsTab->SetSmoothingRealTime( overrides.RealTime );
        if( overrides.bOverride_CatchUp )       mStrokeOptionsTab->SetSmoothingCatchUp( overrides.CatchUp );
        if( overrides.bOverride_Size )          mTopTab->SetSize( overrides.Size );
        if( overrides.bOverride_Opacity )       mTopTab->SetOpacity( overrides.Opacity );
        if( overrides.bOverride_Flow )          mTopTab->SetFlow( overrides.Flow );
        if( overrides.bOverride_BlendingMode )  mTopTab->SetBlendingMode( ( ::ULIS::eBlendingMode )overrides.BlendingMode );
        if( overrides.bOverride_AlphaMode )     mTopTab->SetAlphaMode( ( ::ULIS::eAlphaMode )overrides.AlphaMode );
    }
}

void
FOdysseyPainterEditorToolkit::OnBrushChanged( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );
}

void
FOdysseyPainterEditorToolkit::OnBrushCompiled( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast<UOdysseyBrush*>( iBrush );

    // Reload instance
    if( check_brush )
    {
        if( mBrushInstance )
        {
            if( mBrushInstance->IsValidLowLevel() )
                mBrushInstance->RemoveFromRoot();

            mBrushInstance = NULL;
        }

        //brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        mBrushInstance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), mBrush->GeneratedClass );
        mBrushInstance->AddToRoot();

        mPaintEngine.SetBrushInstance( mBrushInstance );
        mBrushExposedParametersTab->Refresh( mBrushInstance );
    }
}

FOdysseyPaintEngine*
FOdysseyPainterEditorToolkit::PaintEngine()
{
    return &mPaintEngine;
}

FOdysseyLayerStack*
FOdysseyPainterEditorToolkit::LayerStack()
{
    return &mLayerStack;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Mesh Selector

void
FOdysseyPainterEditorToolkit::OnMeshSelected( UStaticMesh* iMesh )
{
}

void
FOdysseyPainterEditorToolkit::OnMeshChanged( UBlueprint* iMesh )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------- Transaction ( Undo / Redo ) methods overrides
void
FOdysseyPainterEditorToolkit::BeginTransaction( const FText& iSessionName )
{
}

void
FOdysseyPainterEditorToolkit::MarkTransactionAsDirty()
{
}

void
FOdysseyPainterEditorToolkit::EndTransaction()
{
}

void
FOdysseyPainterEditorToolkit::SetTextureDirty( bool iIsTextureDirty )
{
    mIsTextureDirty = iIsTextureDirty;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyPainterEditorToolkit::HandleBrushParameterChanged()
{
    mPaintEngine.TriggerStateChanged();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers
void
FOdysseyPainterEditorToolkit::HandleSelectorColorChanged( const ::ULIS::CColor& iColor )
{
    if( mColorSlidersTab )
        mColorSlidersTab->SetColor( iColor );

    mPaintEngine.SetColor( iColor );
}

void
FOdysseyPainterEditorToolkit::HandleSlidersColorChanged( const ::ULIS::CColor& iColor )
{
    if( mColorSelectorTab )
        mColorSelectorTab->SetColor( iColor );

    mPaintEngine.SetColor( iColor );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
FOdysseyPainterEditorToolkit::HandleSizeModifierChanged( int32 iValue )
{
    mPaintEngine.SetSizeModifier( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleOpacityModifierChanged( int32 iValue )
{
    mPaintEngine.SetOpacityModifier( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleFlowModifierChanged( int32 iValue )
{
    mPaintEngine.SetFlowModifier( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleBlendingModeModifierChanged( int32 iValue )
{
    mPaintEngine.SetBlendingModeModifier( static_cast<::ULIS::eBlendingMode>( iValue ) );
}

void
FOdysseyPainterEditorToolkit::HandleAlphaModeModifierChanged( int32 iValue )
{
    mPaintEngine.SetAlphaModeModifier( static_cast<::ULIS::eAlphaMode>( iValue ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
void
FOdysseyPainterEditorToolkit::HandleStrokeStepChanged( int32 iValue )
{
    mPaintEngine.SetStrokeStep( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleStrokeAdaptativeChanged( bool iValue )
{
    mPaintEngine.SetStrokeAdaptative( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleStrokePaintOnTickChanged( bool iValue )
{
    mPaintEngine.SetStrokePaintOnTick( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleInterpolationTypeChanged( int32 iValue )
{
    mPaintEngine.SetInterpolationType( static_cast<EOdysseyInterpolationType>( iValue ) );
}

void
FOdysseyPainterEditorToolkit::HandleSmoothingMethodChanged( int32 iValue )
{
    mPaintEngine.SetSmoothingMethod( static_cast<EOdysseySmoothingMethod>( iValue ) );
}

void
FOdysseyPainterEditorToolkit::HandleSmoothingStrengthChanged( int32 iValue )
{
    mPaintEngine.SetSmoothingStrength( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleSmoothingEnabledChanged( bool iValue )
{
    mPaintEngine.SetSmoothingEnabled( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleSmoothingRealTimeChanged( bool iValue )
{
    mPaintEngine.SetSmoothingRealTime( iValue );
}

void
FOdysseyPainterEditorToolkit::HandleSmoothingCatchUpChanged( bool iValue )
{
    mPaintEngine.SetSmoothingCatchUp( iValue );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers
void
FOdysseyPainterEditorToolkit::HandlePerformanceLiveUpdateChanged( bool iValue )
{
    mLiveUpdateInfo.enabled = iValue;
    mDisplaySurface->Invalidate();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks
TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBrushSelectorTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "BrushSelectorTitle", "BrushSelector" ) )
        [
            SNew( SBox )
            .HeightOverride( 50 )
            [
                mBrushSelectorTab.ToSharedRef()
            ]
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smMeshSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "MeshSelectorTitle", "MeshSelector" ) )
        [
            mMeshSelectorTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBrushExposedParametersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "BrushExposedParametersTitle", "Brush Exposed Parameters" ) )
        [
            mBrushExposedParametersTab.ToSharedRef()
        ];
}

TSharedRef<SDockTab>
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smLayerStackTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "LayerStackTitle", "LayerStack" ) )
        [
            mLayerStackTab.ToSharedRef()
        ];

}

TSharedRef<SDockTab>
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smViewportTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ViewportTabTitle", "Viewport" ) )
        [
            mViewportTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSelectorTitle", "Color Selector" ) )
        [
            mColorSelectorTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSliders( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSlidersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSlidersTitle", "Color Sliders" ) )
        [
            mColorSlidersTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBotBar( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBotBarTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "BotBarTitle", "BotBar" ) )
        [
            SNew( SBox )
            .HeightOverride( 20 )
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTopBar( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smTopBarTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "TopBarTitle", "TopBar" ) )
        [
            mTopTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnStrokeOptions( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smStrokeOptionsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "StrokeOptionsTitle", "Stroke Options" ) )
        [
            mStrokeOptionsTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnPerformanceOptions( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smPerformanceOptionsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "PerformanceOptionsTitle", "Performance Options" ) )
        [
            mPerformanceOptionsTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnNotes( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smNotesTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Notes Title", "Notes" ) )
        [
            SNew( SMultiLineEditableText )
        ];
}

/*
TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnUndoHistory( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smUndoHistoryTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Undo History Title", "Undo History" ) )
        [
            mUndoHistoryTab.ToSharedRef()
        ];
}*/

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTools( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smToolsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Tool Box", "Tools" ) )
        [
            SNew( SScrollBox )
            .Orientation( Orient_Vertical )
            .ScrollBarAlwaysVisible( false )
            +SScrollBox::Slot()
            [
                SNew( SExpandableArea )
                .HeaderContent()
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "Utils", "Utils" ) )
                    .Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
                    .ShadowOffset( FVector2D( 1.0f, 1.0f ) )
                ]
                .BodyContent()
                [
                    SNew( SWrapBox )
                    .UseAllottedWidth( true )
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked( this, &FOdysseyPainterEditorToolkit::OnClearCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Shredder32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ColorPicker32" ) )
                        ]
                    ]
                ]
            ]
            +SScrollBox::Slot()
            [
                SNew( SExpandableArea )
                .HeaderContent()
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "Shapes", "Shapes" ) )
                    .Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
                    .ShadowOffset( FVector2D( 1.0f, 1.0f ) )
                ]
                .BodyContent()
                [
                    SNew( SWrapBox )
                    .UseAllottedWidth( true )
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Line32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Square32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Circle32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Ellipse32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Curve32" ) )
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked( this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer )
                        [
                            SNew( SImage )
                            .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Bezier32" ) )
                        ]
                    ]
                ]
            ]
            +SScrollBox::Slot()
            [
                SNew( SExpandableArea )
                .HeaderContent()
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "UndoRedo  (Experimental)", "UndoRedo  (Experimental)" ) )
                    .Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
                    .ShadowOffset( FVector2D( 1.0f, 1.0f ) )
                ]
                .BodyContent()
                [
                    SNew( SWrapBox )
                    .UseAllottedWidth( true )
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnUndo)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Undo32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnRedo)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Redo32"))
                        ]
                    ]
                    //Debug Purposes
                    /*+SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Check", "Check" ) )
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnCheck)
                    ]*/
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Clear Undo History", "Clear Undo History" ) )
                        .ToolTipText( LOCTEXT( "Clear Undos tooltip", "If the undo/redo is slow, clear the cache by clicking this button" ))
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnClearUndo)
                    ]
                 ]
             ]
        ];
}

FReply
FOdysseyPainterEditorToolkit::OnClearCurrentLayer()
{
    //Record
    mLayerStack.mDrawingUndo->StartRecord();
    mLayerStack.mDrawingUndo->SaveData( 0, 0, mLayerStack.Width(), mLayerStack.Height() );
    mLayerStack.mDrawingUndo->EndRecord();
    //EndRecord
    mIsTextureDirty = true;
    mPaintEngine.AbortStroke();
    mLayerStack.ClearCurrentLayer();
    InvalidateTextureFromData( mLayerStack.GetResultBlock(), mTexture );
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolkit::OnFillCurrentLayer()
{
    //Record
    mLayerStack.mDrawingUndo->StartRecord();
    mLayerStack.mDrawingUndo->SaveData( 0, 0, mLayerStack.Width(), mLayerStack.Height() );
    mLayerStack.mDrawingUndo->EndRecord();
    //EndRecord
    mIsTextureDirty = true;
    mPaintEngine.AbortStroke();
    mLayerStack.FillCurrentLayerWithColor( mPaintEngine.GetColor() );
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolkit::OnClearUndo()
{
    mLayerStack.mDrawingUndo->Clear();
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolkit::OnUndo()
{
    UndoIliad();
    return FReply::Handled();
}


FReply
FOdysseyPainterEditorToolkit::OnRedo()
{
    RedoIliad();
    return FReply::Handled();
}

//Debug purposes
/*FReply
FOdysseyPainterEditorToolkit::OnCheck()
{
    mLayerStack.mDrawingUndo->Check();
    return FReply::Handled();
}*/

void
FOdysseyPainterEditorToolkit::SetColor( const ::ULIS::CColor& iColor )
{
    if( mColorSelectorTab ) 
        mColorSelectorTab->SetColor( iColor );
}

void
FOdysseyPainterEditorToolkit::UndoIliad()
{
    mPaintEngine.InterruptStrokeAndStampInPlace();
    mLayerStack.mDrawingUndo->LoadData();
}


void
FOdysseyPainterEditorToolkit::RedoIliad()
{
    mPaintEngine.InterruptStrokeAndStampInPlace();
    mLayerStack.mDrawingUndo->Redo();
}

#undef LOCTEXT_NAMESPACE
