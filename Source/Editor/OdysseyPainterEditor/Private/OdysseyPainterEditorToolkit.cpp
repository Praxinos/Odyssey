// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyPainterEditorToolkit.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Layout/SBox.h"
#include "Interfaces/IOdysseyPainterEditorModule.h"
#include "Slate/SceneViewport.h"
#include "PropertyEditorModule.h"
#include "OdysseyPainterEditorConstants.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "ISettingsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SSlider.h"
#include "Dialogs/Dialogs.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "OdysseyStyleSet.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistryModule.h"

#include "SOdysseySurfaceViewport.h"
#include "OdysseyPainterEditorViewportClient.h"
#include "OdysseyImageLayer.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBlock.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyAboutScreen.h"
#include <ULIS_CORE>


#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"


const FName FOdysseyPainterEditorToolkit::ViewportTabId(                TEXT( "OdysseyPainterEditor_Viewport" ) );
const FName FOdysseyPainterEditorToolkit::BrushSelectorTabId(           TEXT( "OdysseyPainterEditor_BrushSelector" ) );
const FName FOdysseyPainterEditorToolkit::MeshSelectorTabId(            TEXT( "OdysseyPainterEditor_MeshSelector" ) );
const FName FOdysseyPainterEditorToolkit::BrushExposedParametersTabId(  TEXT( "OdysseyPainterEditor_BrushExposedParameters" ) );
const FName FOdysseyPainterEditorToolkit::ColorSelectorTabId(           TEXT( "OdysseyPainterEditor_ColorSelector" ) );
const FName FOdysseyPainterEditorToolkit::ColorSlidersTabId(            TEXT( "OdysseyPainterEditor_ColorSliders" ) );
const FName FOdysseyPainterEditorToolkit::LayerStackTabId(              TEXT( "OdysseyPainterEditor_LayerStack" ) );
const FName FOdysseyPainterEditorToolkit::BotBarTabId(                  TEXT( "OdysseyPainterEditor_BotBar" ) );
const FName FOdysseyPainterEditorToolkit::TopBarTabId(                  TEXT( "OdysseyPainterEditor_TopBar" ) );
const FName FOdysseyPainterEditorToolkit::StrokeOptionsTabId(           TEXT( "OdysseyPainterEditor_StrokeOptions" ) );
const FName FOdysseyPainterEditorToolkit::NotesTabId(                   TEXT( "OdysseyPainterEditor_Notes" ) );
const FName FOdysseyPainterEditorToolkit::UndoHistoryTabId(             TEXT( "OdysseyPainterEditor_UndoHistory" ) );
const FName FOdysseyPainterEditorToolkit::PerformanceOptionsTabId(      TEXT( "OdysseyPainterEditor_PerformanceOptions" ) );
const FName FOdysseyPainterEditorToolkit::ToolsTabId(                   TEXT( "OdysseyPainterEditor_Tools"));


const FName OdysseyPainterEditorAppIdentifier = FName( TEXT( "OdysseyPainterEditorApp" ) );


/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolkit::~FOdysseyPainterEditorToolkit()
{
    //GEditor->UnregisterForUndo(this);
    if( brush_instance )
    {
        brush_instance->CleansePool( ECacheLevel::kSuper );
        brush_instance->CleansePools();
        brush_instance->RemoveFromRoot();
        brush_instance = NULL;
    }
    if( displaySurface ) delete  displaySurface;
}


FOdysseyPainterEditorToolkit::FOdysseyPainterEditorToolkit()
    : scopedTransaction(                NULL )
    , UndoHistory()
    , bManipulationDirtiedSomething(    false )
    , bEditorMarkedAsClosed(            false )
    , texture(                          NULL )
    , displaySurface(                   NULL )
    , textureContentsBackup(            NULL )
    , textureGroupBackup()
    , paintEngine(                      &UndoHistory )
    , layer_stack()
    , brush(                            NULL )
    , brush_instance(                   NULL )
{}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorToolkit::InitOdysseyPainterEditor( const EToolkitMode::Type Mode,
                                                        const TSharedPtr< class IToolkitHost >& InitToolkitHost,
                                                        UTexture2D* iTexture )
{
    // Setup Texture
    texture = iTexture;
    textureGroupBackup              = texture->LODGroup;
    texture->MipGenSettings         = TextureMipGenSettings::TMGS_NoMipmaps;
    texture->CompressionSettings    = TextureCompressionSettings::TC_VectorDisplacementmap;
    texture->LODGroup               = TextureGroup::TEXTUREGROUP_Pixels2D;
    texture->UpdateResource();
    textureContentsBackup = NewOdysseyBlockFromUTextureData( texture );

    // Setup Layers
    layer_stack.InitFromData( textureContentsBackup );
    layer_stack.ComputeResultBlock();

    // Setup Paint Engine
    paintEngine.SetLayerStack(         &layer_stack );
    paintEngine.SetBrushInstance(      NULL );
    paintEngine.SetColor(              ::ULIS::CColor() );
    paintEngine.SetSizeModifier(       20.f );

    // Setup Surface
    displaySurface = new FOdysseySurface( layer_stack.GetResultBlock() );
    liveUpdateInfo.main = displaySurface->Texture();
    liveUpdateInfo.live = texture;
    liveUpdateInfo.enabled = false;
    displaySurface->Block()->GetIBlock()->SetInvalidateCB( &InvalidateLiveSurfaceCallback, static_cast< void* >( &liveUpdateInfo ) );

    displaySurface->Invalidate();

    // Support undo/redo
    displaySurface->Texture()->SetFlags(RF_Transactional);
    GEditor->RegisterForUndo(this);

    // Register our commands. This will only register them if not previously registered
    FOdysseyPainterEditorCommands::Register();

    // Build commands
    BindCommands();

    // Create tabs contents
    CreateMeshSelectorTab();
    CreateViewportTab();
    CreateBrushSelectorTab();
    CreateBrushExposedParametersTab();
    CreateLayerStackTab(); //layer_stack have to be initialized at this point
    CreateColorSelectorTab();
    CreateColorSlidersTab();
    CreateTopTab();
    CreateStrokeOptionsTab();
    CreatePerformanceOptionsTab();
    CreateUndoHistoryTab();


    // Setup Properties with callbacks
    ColorSelectorTab->SetColor(             ::ULIS::CColor( 0, 0, 0 ) );

    StrokeOptionsTab->SetStrokeStep         ( 20 );
    StrokeOptionsTab->SetStrokeAdaptative   ( true );
    StrokeOptionsTab->SetStrokePaintOnTick  ( false );
    StrokeOptionsTab->SetInterpolationType  ( (int32)EOdysseyInterpolationType::kBezier );
    StrokeOptionsTab->SetSmoothingMethod    ( (int32)EOdysseySmoothingMethod::kAverage );
    StrokeOptionsTab->SetSmoothingStrength  ( 10 );
    StrokeOptionsTab->SetSmoothingEnabled   ( true );
    StrokeOptionsTab->SetSmoothingRealTime  ( true );
    StrokeOptionsTab->SetSmoothingCatchUp   ( true );

    PerformanceOptionsTab->SetPerformanceOptionLiveUpdate( true );

    TopTab->SetSize(                        20 );
    TopTab->SetOpacity(                     100 );
    TopTab->SetFlow(                        100 );

    // Build Layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_OdysseyPainterEditor_Layout")
    ->AddArea
    (
        FTabManager::NewPrimaryArea()
        ->SetOrientation(Orient_Horizontal)
        ->Split
        (
            FTabManager::NewSplitter()
            ->SetOrientation(Orient_Vertical)
            ->SetSizeCoefficient(1.f)
            // TopMost Part
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetSizeCoefficient(1.f)
                ->SetOrientation(Orient_Horizontal)
                // Left Bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Vertical)
                    ->SetSizeCoefficient( 0.2f )
                    // Brush Selector
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( BrushSelectorTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.1f)
                    )
                    // Brush Preview
                    ->Split
                    (
                        FTabManager::NewStack()
                        /*->AddTab( FName( "BrushPreview" ), ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.1f)*/
                        // Brush Params
                        ->AddTab( BrushExposedParametersTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.1f)
                    )
                    // Brush Params + Stroke Options
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.3f)
                        // Stroke Options
                        ->AddTab( StrokeOptionsTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient(0.3f)
                        // Performance Options
                        ->AddTab( PerformanceOptionsTabId, ETabState::OpenedTab )
                        ->SetHideTabWell( false )
                        ->SetSizeCoefficient(0.3f)
                        // Mesh Selector
                        ->AddTab( MeshSelectorTabId, ETabState::ClosedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.3f)
                    )
                    // Tools
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( ToolsTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.2f)
                    )
                    // Navigator
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( FName( "Navigator" ), ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.3f)

                        // Notes
                        ->AddTab( NotesTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.3f)
                    )
                )
                // Middle bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Vertical)
                    // Top Bar
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( TopBarTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(true)
                        ->SetSizeCoefficient(0.1f)
                    )
                    // Viewport
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( ViewportTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.9f)
                    )
                )
                // Right bar
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetSizeCoefficient(0.16f)
                    ->SetOrientation(Orient_Vertical)
                    // ColorSelector
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( ColorSelectorTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.2f)
                    )
                    // ColorSliders
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->AddTab( ColorSlidersTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.2f)
                    )
                    // LayerStack + Notes
                    ->Split
                    (
                        FTabManager::NewStack()
                        // Undo History
                        ->AddTab( UndoHistoryTabId, ETabState::ClosedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.6f)
                        // Layer Stack
                        ->AddTab( LayerStackTabId, ETabState::OpenedTab)
                        ->SetHideTabWell(false)
                        ->SetSizeCoefficient(0.6f)
                    )
                )
            )
            // Bottom Part
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab( FName( BotBarTabId ), ETabState::OpenedTab)
                ->SetHideTabWell(true)
            )
        )
    );



    IOdysseyPainterEditorModule* OdysseyPainterEditorModule = &FModuleManager::LoadModuleChecked<IOdysseyPainterEditorModule>("OdysseyPainterEditor");

    FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, OdysseyPainterEditorAppIdentifier, StandaloneDefaultLayout, true, false, texture );

    InitializeExtenders();

    RegenerateMenusAndToolbars();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
void
FOdysseyPainterEditorToolkit::RegisterTabSpawners( const TSharedRef< class FTabManager >& InTabManager )
{
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyPainterEditor", "Odyssey Painter Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

    //The viewport: the central area where we can draw on images
    InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnViewport))
        .SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.Viewport16"));

    // BrushSelector
    InTabManager->RegisterTabSpawner(BrushSelectorTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushSelector))
        .SetDisplayName(LOCTEXT("BrushSelectorTab", "Brush Selector") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.BrushSelector16"));

    // MeshSelector
    InTabManager->RegisterTabSpawner(MeshSelectorTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnMeshSelector))
        .SetDisplayName(LOCTEXT("MeshSelectorTab", "Mesh Selector") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.Mesh16"));

    // BrushExposedParameters
    InTabManager->RegisterTabSpawner(BrushExposedParametersTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushExposedParameters))
        .SetDisplayName(LOCTEXT("BrushExposedParametersTab", "Brush Exposed Parameters") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.BrushExposedParameters16"));

    // ColorSelector
    InTabManager->RegisterTabSpawner(ColorSelectorTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSelector))
        .SetDisplayName(LOCTEXT("ColorSelectorTab", "ColorSelector") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.ColorWheel16"));

    // ColorSliders
    InTabManager->RegisterTabSpawner(ColorSlidersTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSliders))
        .SetDisplayName(LOCTEXT("ColorSlidersTab", "ColorSliders") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.ColorSliders_2_16"));

    // LayerStack
    InTabManager->RegisterTabSpawner(LayerStackTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnLayerStack))
        .SetDisplayName(LOCTEXT("LayerStackTab", "LayerStack"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.Layers16"));

    // BotBar
    InTabManager->RegisterTabSpawner(BotBarTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBotBar))
        .SetDisplayName(LOCTEXT("BotBarTab", "BotBar") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.BotBar16"));

    // TopBar
    InTabManager->RegisterTabSpawner(TopBarTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTopBar))
        .SetDisplayName(LOCTEXT("TopBarTab", "TopBar") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.TopBar16"));

    // StrokeOptions
    InTabManager->RegisterTabSpawner(StrokeOptionsTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnStrokeOptions))
        .SetDisplayName(LOCTEXT("StrokeOptionsTab", "Stroke Options") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.StrokeOptions16"));

    // PerformanceOptions
    InTabManager->RegisterTabSpawner(PerformanceOptionsTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnPerformanceOptions))
        .SetDisplayName(LOCTEXT("PerformanceOptionsTab", "Performance Options") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.PerformanceTools16"));

    // Notes
    InTabManager->RegisterTabSpawner(NotesTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnNotes))
        .SetDisplayName(LOCTEXT("NotesTab", "Notes") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.Notes16"));

    // Undo History
    InTabManager->RegisterTabSpawner(UndoHistoryTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnUndoHistory))
        .SetDisplayName(LOCTEXT("UndoHistoryTab", "UndoHistory") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.UndoHistory16"));

    // Tools
    InTabManager->RegisterTabSpawner(ToolsTabId, FOnSpawnTab::CreateSP(this, &FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTools))
        .SetDisplayName(LOCTEXT("ToolsTab", "Tools") )
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon("OdysseyStyle", "PainterEditor.Tools16"));
}


void
FOdysseyPainterEditorToolkit::UnregisterTabSpawners( const TSharedRef< class FTabManager >& InTabManager )
{
    FAssetEditorToolkit::UnregisterTabSpawners( InTabManager );
    InTabManager->UnregisterTabSpawner( ViewportTabId );
    InTabManager->UnregisterTabSpawner( BrushSelectorTabId );
    InTabManager->UnregisterTabSpawner( MeshSelectorTabId );
    InTabManager->UnregisterTabSpawner( BrushExposedParametersTabId );
    InTabManager->UnregisterTabSpawner( ColorSelectorTabId );
    InTabManager->UnregisterTabSpawner( ColorSlidersTabId );
    InTabManager->UnregisterTabSpawner( LayerStackTabId );
    InTabManager->UnregisterTabSpawner( BotBarTabId );
    InTabManager->UnregisterTabSpawner( TopBarTabId );
    InTabManager->UnregisterTabSpawner( StrokeOptionsTabId );
    InTabManager->UnregisterTabSpawner( PerformanceOptionsTabId );
    InTabManager->UnregisterTabSpawner( NotesTabId );
    InTabManager->UnregisterTabSpawner( UndoHistoryTabId );
    InTabManager->UnregisterTabSpawner( ToolsTabId );
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
FText
FOdysseyPainterEditorToolkit::GetBaseToolkitName() const
{
    return  LOCTEXT( "AppLabel", "Odyssey Painter Editor" );
}


FName
FOdysseyPainterEditorToolkit::GetToolkitFName() const
{
    return  FName( "OdysseyPainterEditor" );
}


FLinearColor
FOdysseyPainterEditorToolkit::GetWorldCentricTabColorScale( ) const
{
    return  FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}


FString
FOdysseyPainterEditorToolkit::GetWorldCentricTabPrefix( ) const
{
    return  LOCTEXT( "WorldCentricTabPrefix", "Texture" ).ToString();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FEditorUndoClient interface
void
FOdysseyPainterEditorToolkit::PostUndo( bool bSuccess )
{
    TArray< TSharedPtr< IOdysseyLayer > >* layers = LayerStack()->GetLayers();
    for( int i = 0; i < layers->Num(); i++ )
    {
        FOdysseyImageLayer* imageLayer = static_cast< FOdysseyImageLayer* >( (*layers)[i].Get() );
        CopyUTextureDataIntoBlock( imageLayer->GetBlock(), imageLayer->mTexture );
    }
    LayerStack()->ComputeResultBlock();
    
    /*UE_LOG(LogTemp, Display, TEXT("Texture: %p"), displaySurface->Texture() );
    UE_LOG(LogTemp, Display, TEXT("block: %p"), displaySurface->Block() );

    CopyUTextureDataIntoBlock( displaySurface->Block(), displaySurface->Texture() );
    
    displaySurface->Invalidate();*/

    //layer_stack.ComputeResultBlock();
    //displaySurface->Invalidate();
    //CopyBlockDataIntoUTexture( displaySurface->Block(), texture );
    //::ULIS::FMakeContext::CopyBlockInto( displaySurface->Block()->GetIBlock(), textureContentsBackup->GetIBlock() );
    //InvalidateTextureFromData( displaySurface->Block(), texture );

    //FOdysseyBlock* block = NewOdysseyBlockFromUTextureData( displaySurface->Texture() );
    //CopyBlockDataIntoUTexture( block, displaySurface->Texture() );
    //InvalidateTextureFromData( block, displaySurface->Texture() );
    // Invalidate all
    //displaySurface->Invalidate();
    //delete block;
}


void
FOdysseyPainterEditorToolkit::PostRedo( bool bSuccess )
{
    PostUndo(bSuccess);
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override
void
FOdysseyPainterEditorToolkit::SaveAsset_Execute()
{
    // Commit changes permanently
    //displaySurface->CommitBlockChangesIntoTextureBulk();
    // Reload backup
    
    BeginTransaction( LOCTEXT("Save in ILIAD", "Save in ILIAD") );
    MarkTransactionAsDirty();
    
    CopyBlockDataIntoUTexture( displaySurface->Block(), texture );
    ::ULIS::FMakeContext::CopyBlockInto( displaySurface->Block()->GetIBlock(), textureContentsBackup->GetIBlock() );
    InvalidateTextureFromData( displaySurface->Block(), texture );
    // Invalidate all
    displaySurface->Invalidate();

    FAssetEditorToolkit::SaveAsset_Execute();

    EndTransaction();
}


bool
FOdysseyPainterEditorToolkit::OnRequestClose()
{
    if( !bEditorMarkedAsClosed )
    {
        EAppReturnType::Type returnType =
            OpenMsgDlgInt( EAppMsgType::YesNoCancel,
                           FText::Format( LOCTEXT( "Save Texture Prompt",
                                                   "Save the texture {0} before exiting ? Warning, when closing the texture, layers will be merged, export them first if you want to keep them ! (File/Export Layers)" ),
                                          FText::FromString( texture->GetFName().ToString() ) ),
                                          LOCTEXT( "Save Texture Title", "Save Texture" ) );

        if( returnType == EAppReturnType::Cancel )
            return  false;

        if( returnType == EAppReturnType::Yes )
            SaveAsset_Execute();

        // Invalidate All from backup data
        // If saved, no change
        // If unsaved, revert display to last saved data
        InvalidateTextureFromData( textureContentsBackup, texture );
        InvalidateSurfaceFromData( textureContentsBackup, displaySurface );
    }
    texture->LODGroup = textureGroupBackup;
    bEditorMarkedAsClosed = true;
    return true;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building
void
FOdysseyPainterEditorToolkit::BindCommands( )
{
    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers,
        FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolkit::OnImportTexturesAsLayers),
        FCanExecuteAction());

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures,
        FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolkit::OnExportLayersAsTextures),
        FCanExecuteAction());

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().AboutIliad,
        FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolkit::OnAboutIliad),
        FCanExecuteAction());

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite,
        FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolkit::OnVisitPraxinosWebsite),
        FCanExecuteAction());

    ToolkitCommands->MapAction(
        FOdysseyPainterEditorCommands::Get().VisitPraxinosForums,
        FExecuteAction::CreateSP(this, &FOdysseyPainterEditorToolkit::OnVisitPraxinosForums),
        FCanExecuteAction());
}

void
FOdysseyPainterEditorToolkit::InitializeExtenders()
{
    // Create the Extender that will add content to the menu
    TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
    SetupOdysseyPainterEditorMenu( MenuExtender, *this );
    AddMenuExtender(MenuExtender);

    IOdysseyPainterEditorModule* OdysseyPainterEditorModule = &FModuleManager::LoadModuleChecked<IOdysseyPainterEditorModule>("OdysseyPainterEditor");
}

void
FOdysseyPainterEditorToolkit::SetupOdysseyPainterEditorMenu( TSharedPtr< FExtender > Extender, FOdysseyPainterEditorToolkit& OdysseyPainterEditor)
{
        Extender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
        OdysseyPainterEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorToolkit& >( &FOdysseyPainterEditorToolkit::FillImportExportMenu, OdysseyPainterEditor ) );


        Extender->AddMenuExtension(
        "HelpApplication",
        EExtensionHook::After,
        OdysseyPainterEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic< FOdysseyPainterEditorToolkit& >( &FOdysseyPainterEditorToolkit::FillAboutMenu, OdysseyPainterEditor ) );
}

//static
void
FOdysseyPainterEditorToolkit::FillImportExportMenu( FMenuBuilder& MenuBuilder, FOdysseyPainterEditorToolkit& Kismet )
{
    MenuBuilder.BeginSection("FileOdysseyPainter", LOCTEXT("OdysseyPainter", "OdysseyPainter"));
    {
        MenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ImportTexturesAsLayers );
        MenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ExportLayersAsTextures );
    }
}

//static
void
FOdysseyPainterEditorToolkit::FillAboutMenu( FMenuBuilder& MenuBuilder, FOdysseyPainterEditorToolkit& Kismet )
{
    MenuBuilder.BeginSection("About", LOCTEXT("OdysseyPainter", "OdysseyPainter"));
    {
        MenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().AboutIliad );
        MenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite );
        MenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosForums );
    }
}

void
FOdysseyPainterEditorToolkit::OnExportLayersAsTextures()
{
    FSaveAssetDialogConfig SaveAssetDialogConfig;
    SaveAssetDialogConfig.DialogTitleOverride = LOCTEXT("ExportLayerDialogTitle", "Export Layers As Texture");
    SaveAssetDialogConfig.DefaultPath = FPaths::GetPath(texture->GetPathName());
    SaveAssetDialogConfig.DefaultAssetName = texture->GetName();
    SaveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );
    SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);

    //UE_LOG(LogTemp, Display, TEXT("%s"), *(FPaths::GetBaseFilename(SaveObjectPath)) );


    if( SaveObjectPath!= "" )
    {
        TArray<TSharedPtr<IOdysseyLayer>>* layers = layer_stack.GetLayers();

        for( int i = 0; i < layers->Num(); i++ )
        {
            if( !((*layers)[i].Get()->GetType() == IOdysseyLayer::eType::kImage) )
                continue;

            FOdysseyImageLayer* imageLayer = static_cast<FOdysseyImageLayer*> ((*layers)[i].Get());

            FString AssetPath = FPaths::GetPath(SaveObjectPath) + "/";
            FString PackagePath = (AssetPath + imageLayer->GetName().ToString().Replace(TEXT(" "), TEXT("_")));
            UPackage *Package = CreatePackage(nullptr, *PackagePath);

            UTexture2D* Object = NewObject<UTexture2D>( Package, UTexture2D::StaticClass(), FName( *(FPaths::GetBaseFilename(SaveObjectPath) + TEXT("_") + imageLayer->GetName().ToString() ) ), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
            Object->Source.Init( imageLayer->GetBlock()->Width(), imageLayer->GetBlock()->Height(), 1, 1, TSF_BGRA8 );
            Object->PostEditChange();

            CopyBlockDataIntoUTexture( imageLayer->GetBlock(), Object );

            Object->UpdateResource();

            FAssetRegistryModule::AssetCreated(Object);
            Object->MarkPackageDirty();

            bool bSuccess = UPackage::SavePackage(Package, Object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *(imageLayer->GetName().ToString()));
        }
    }
}

void
FOdysseyPainterEditorToolkit::OnImportTexturesAsLayers()
{
    FOpenAssetDialogConfig OpenAssetDialogConfig;
    OpenAssetDialogConfig.DialogTitleOverride = LOCTEXT("ImportTextureDialogTitle", "Import Textures As Layers");
    OpenAssetDialogConfig.DefaultPath = FPaths::GetPath(texture->GetPathName());
    OpenAssetDialogConfig.bAllowMultipleSelection = true;
    OpenAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetFName() );


    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray < FAssetData > AssetsData = ContentBrowserModule.Get().CreateModalOpenAssetDialog(OpenAssetDialogConfig);

    for (int i = 0; i < AssetsData.Num(); i++ )
    {
        //UE_LOG(LogTemp, Display, TEXT("%s"), *(AssetsData[i].GetFullName()) );
        UTexture2D* openedTexture = static_cast< UTexture2D* >( AssetsData[i].GetAsset() );
        FOdysseyBlock* textureBlock = NewOdysseyBlockFromUTextureData( openedTexture );
        layer_stack.AddLayerFromData( textureBlock, FName( *(openedTexture->GetName()) ) );
        delete  textureBlock;
    }

    LayerStackTab->RefreshView();
    layer_stack.ComputeResultBlock();
}


void
FOdysseyPainterEditorToolkit::OnAboutIliad()
{
    const FText AboutWindowTitle = LOCTEXT( "AboutIliad", "About Iliad" );

    TSharedPtr<SWindow> aboutWindow =
        SNew(SWindow)
        .Title( AboutWindowTitle )
        .ClientSize(FVector2D(600.f, 300.f))
        .SupportsMaximize(false) .SupportsMinimize(false)
        .SizingRule( ESizingRule::FixedSize )
        [
            SNew(SOdysseyAboutScreen)
        ];

        FSlateApplication::Get().AddModalWindow(aboutWindow.ToSharedRef(), this->GetToolkitHost()->GetParentWidget() );
}

void
FOdysseyPainterEditorToolkit::OnVisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

void
FOdysseyPainterEditorToolkit::OnVisitPraxinosForums()
{
    FString URL = "https://praxinos.coop/forum";
    FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation
void
FOdysseyPainterEditorToolkit::CreateViewportTab()
{
    ViewportTab = SNew( SOdysseySurfaceViewport );
    ViewportTab->SetSurface( displaySurface );
    ViewportTab->SetViewportClient( MakeShareable( new FOdysseyPainterEditorViewportClient( SharedThis( this ), ViewportTab, MeshSelectorTab->GetMeshSelectorPtr() ) ) );
}


void
FOdysseyPainterEditorToolkit::CreateBrushSelectorTab()
{
    BrushSelectorTab = SNew( SOdysseyBrushSelector )
        .OnBrushChanged( this, &FOdysseyPainterEditorToolkit::OnBrushSelected );
}

void
FOdysseyPainterEditorToolkit::CreateMeshSelectorTab()
{
    MeshSelectorTab = SNew( SOdysseyMeshSelector )
        .OnMeshChanged( this, &FOdysseyPainterEditorToolkit::OnMeshSelected );
}

void
FOdysseyPainterEditorToolkit::CreateBrushExposedParametersTab()
{
    BrushExposedParametersTab = SNew( SOdysseyBrushExposedParameters )
        .OnParameterChanged( this, &FOdysseyPainterEditorToolkit::HandleBrushParameterChanged );
}

void
FOdysseyPainterEditorToolkit::CreateLayerStackTab()
{
    LayerStackTab = SNew( SOdysseyLayerStackView )
                    .LayerStackData(MakeShareable( &layer_stack ));
}


void
FOdysseyPainterEditorToolkit::CreateColorSelectorTab()
{
    ColorSelectorTab = SNew( SOdysseyColorSelector ).OnColorChanged( this, &FOdysseyPainterEditorToolkit::HandleSelectorColorChanged );
}

void
FOdysseyPainterEditorToolkit::CreateColorSlidersTab()
{
    ColorSlidersTab = SNew( SOdysseyColorSliders ).OnColorChanged( this, &FOdysseyPainterEditorToolkit::HandleSlidersColorChanged );
}

void
FOdysseyPainterEditorToolkit::CreateToolsTab()
{

}


void
FOdysseyPainterEditorToolkit::CreateTopTab()
{
    TopTab = SNew( SOdysseyPaintModifiers )
        .OnSizeChanged(             this, &FOdysseyPainterEditorToolkit::HandleSizeModifierChanged )
        .OnOpacityChanged(          this, &FOdysseyPainterEditorToolkit::HandleOpacityModifierChanged )
        .OnFlowChanged(             this, &FOdysseyPainterEditorToolkit::HandleFlowModifierChanged )
        .OnBlendingModeChanged(     this, &FOdysseyPainterEditorToolkit::HandleBlendingModeModifierChanged );
}

void
FOdysseyPainterEditorToolkit::CreateStrokeOptionsTab()
{
    StrokeOptionsTab = SNew( SOdysseyStrokeOptions )
        .OnStrokeStepChanged        (   this,   &FOdysseyPainterEditorToolkit::HandleStrokeStepChanged          )
        .OnStrokeAdaptativeChanged  (   this,   &FOdysseyPainterEditorToolkit::HandleStrokeAdaptativeChanged    )
        .OnStrokePaintOnTickChanged (   this,   &FOdysseyPainterEditorToolkit::HandleStrokePaintOnTickChanged   )
        .OnInterpolationTypeChanged (   this,   &FOdysseyPainterEditorToolkit::HandleInterpolationTypeChanged   )
        .OnSmoothingMethodChanged   (   this,   &FOdysseyPainterEditorToolkit::HandleSmoothingMethodChanged     )
        .OnSmoothingStrengthChanged (   this,   &FOdysseyPainterEditorToolkit::HandleSmoothingStrengthChanged   )
        .OnSmoothingEnabledChanged  (   this,   &FOdysseyPainterEditorToolkit::HandleSmoothingEnabledChanged    )
        .OnSmoothingRealTimeChanged (   this,   &FOdysseyPainterEditorToolkit::HandleSmoothingRealTimeChanged   )
        .OnSmoothingCatchUpChanged  (   this,   &FOdysseyPainterEditorToolkit::HandleSmoothingCatchUpChanged    );
}

void
FOdysseyPainterEditorToolkit::CreatePerformanceOptionsTab()
{
    PerformanceOptionsTab = SNew( SOdysseyPerformanceOptions )
        .OnLiveUpdateChanged        (   this,   &FOdysseyPainterEditorToolkit::HandlePerformanceLiveUpdateChanged   );
}

void
FOdysseyPainterEditorToolkit::CreateUndoHistoryTab()
{
    UndoHistoryTab = SNew( SOdysseyUndoHistory, &UndoHistory );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- Paint engine driving methods
void
FOdysseyPainterEditorToolkit::OnBrushSelected( UOdysseyBrush* iBrush )
{
    brush = iBrush;

    if( brush_instance )
    {
        brush_instance->RemoveFromRoot();
        brush_instance = NULL;
    }

    if( brush )
    {
        //@todo: check
        //brush->OnChanged().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushChanged );

        brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        brush_instance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), brush->GeneratedClass );
        brush_instance->AddToRoot();

        paintEngine.SetBrushInstance( brush_instance );
        BrushExposedParametersTab->Refresh( brush_instance );

        FOdysseyBrushPreferencesOverrides& overrides = brush_instance->Preferences;
        if( overrides.bOverride_Step            )   StrokeOptionsTab->SetStrokeStep(        overrides.Step                      );
        if( overrides.bOverride_Adaptative      )   StrokeOptionsTab->SetStrokeAdaptative(  overrides.SizeAdaptative            );
        if( overrides.bOverride_PaintOnTick     )   StrokeOptionsTab->SetStrokePaintOnTick( overrides.PaintOnTick               );
        if( overrides.bOverride_Type            )   StrokeOptionsTab->SetInterpolationType( (int32)overrides.Type               );
        if( overrides.bOverride_Method          )   StrokeOptionsTab->SetSmoothingMethod(   (int32)overrides.Method             );
        if( overrides.bOverride_Strength        )   StrokeOptionsTab->SetSmoothingStrength( overrides.Strength                  );
        if( overrides.bOverride_Enabled         )   StrokeOptionsTab->SetSmoothingEnabled(  overrides.Enabled                   );
        if( overrides.bOverride_RealTime        )   StrokeOptionsTab->SetSmoothingRealTime( overrides.RealTime                  );
        if( overrides.bOverride_CatchUp         )   StrokeOptionsTab->SetSmoothingCatchUp(  overrides.CatchUp                   );
        if( overrides.bOverride_Size            )   TopTab->SetSize(            overrides.Size                                  );
        if( overrides.bOverride_Opacity         )   TopTab->SetOpacity(         overrides.Opacity                               );
        if( overrides.bOverride_Flow            )   TopTab->SetFlow(            overrides.Flow                                  );
        if( overrides.bOverride_BlendingMode    )   TopTab->SetBlendingMode(    (::ULIS::eBlendingMode)overrides.BlendingMode   );
    }
}


void
FOdysseyPainterEditorToolkit::OnBrushChanged( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast< UOdysseyBrush* >( iBrush );
}

void
FOdysseyPainterEditorToolkit::OnBrushCompiled( UBlueprint* iBrush )
{
    UOdysseyBrush* check_brush = dynamic_cast< UOdysseyBrush* >( iBrush );

    // Reload instance
    if( check_brush )
    {
        if( brush_instance )
        {
            if( brush_instance->IsValidLowLevel() )
                brush_instance->RemoveFromRoot();

            brush_instance = NULL;
        }

        //brush->OnCompiled().AddSP( this, &FOdysseyPainterEditorToolkit::OnBrushCompiled );
        brush_instance = NewObject< UOdysseyBrushAssetBase >( GetTransientPackage(), brush->GeneratedClass );
        brush_instance->AddToRoot();

        paintEngine.SetBrushInstance( brush_instance );
        BrushExposedParametersTab->Refresh( brush_instance );
    }
}


FOdysseyPaintEngine*
FOdysseyPainterEditorToolkit::PaintEngine()
{
    return  &paintEngine;
}


FOdysseyLayerStack*
FOdysseyPainterEditorToolkit::LayerStack()
{
    return  &layer_stack;
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
FOdysseyPainterEditorToolkit::BeginTransaction( const FText& SessionName )
{
    if( scopedTransaction == nullptr )
    {
        scopedTransaction = new FScopedTransaction( SessionName );
        TArray< TSharedPtr< IOdysseyLayer > >* layers = LayerStack()->GetLayers();
        for( int i = 0; i < layers->Num(); i++ )
        {
            FOdysseyImageLayer* imageLayer = static_cast< FOdysseyImageLayer* >( (*layers)[i].Get() );
            imageLayer->mTexture->Modify();
        }
    }
}


void
FOdysseyPainterEditorToolkit::MarkTransactionAsDirty()
{
    bManipulationDirtiedSomething = true;
}


void
FOdysseyPainterEditorToolkit::EndTransaction()
{
    if( bManipulationDirtiedSomething )
    {
        TArray< TSharedPtr< IOdysseyLayer > >* layers = LayerStack()->GetLayers();
        for( int i = 0; i < layers->Num(); i++ )
        {
            FOdysseyImageLayer* imageLayer = static_cast< FOdysseyImageLayer* >( (*layers)[i].Get() );
            CopyBlockDataIntoUTexture( imageLayer->GetBlock(), imageLayer->mTexture );
            imageLayer->mTexture->PostEditChange();
        }
        displaySurface->Invalidate();
    }

    bManipulationDirtiedSomething = false;

    if( scopedTransaction != nullptr )
    {
        delete scopedTransaction;
        scopedTransaction = nullptr;
    }
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Brush Handlers
void
FOdysseyPainterEditorToolkit::HandleBrushParameterChanged()
{
    paintEngine.TriggerStateChanged();
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Color Handlers
void
FOdysseyPainterEditorToolkit::HandleSelectorColorChanged( const ::ULIS::CColor& iColor )
{
    if( ColorSlidersTab ) ColorSlidersTab->SetColor( iColor );
    paintEngine.SetColor( iColor );
}


void
FOdysseyPainterEditorToolkit::HandleSlidersColorChanged( const ::ULIS::CColor& iColor )
{
    if( ColorSelectorTab ) ColorSelectorTab->SetColor( iColor );
    paintEngine.SetColor( iColor );
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Modifiers Handlers
void
FOdysseyPainterEditorToolkit::HandleSizeModifierChanged( int32 iValue )
{
    paintEngine.SetSizeModifier( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleOpacityModifierChanged( int32 iValue )
{
    paintEngine.SetOpacityModifier( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleFlowModifierChanged( int32 iValue )
{
    paintEngine.SetFlowModifier( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleBlendingModeModifierChanged( int32 iValue )
{
    paintEngine.SetBlendingModeModifier( static_cast<::ULIS::eBlendingMode>( iValue ) );
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------- Stroke Options Handlers
void
FOdysseyPainterEditorToolkit::HandleStrokeStepChanged           (   int32   iValue  )
{
    paintEngine.SetStrokeStep( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleStrokeAdaptativeChanged     (   bool    iValue  )
{
    paintEngine.SetStrokeAdaptative( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleStrokePaintOnTickChanged    (   bool    iValue  )
{
    paintEngine.SetStrokePaintOnTick( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleInterpolationTypeChanged    (   int32   iValue  )
{
    paintEngine.SetInterpolationType( static_cast< EOdysseyInterpolationType >( iValue ) );
}


void
FOdysseyPainterEditorToolkit::HandleSmoothingMethodChanged      (   int32   iValue  )
{
    paintEngine.SetSmoothingMethod( static_cast< EOdysseySmoothingMethod >( iValue ) );
}


void
FOdysseyPainterEditorToolkit::HandleSmoothingStrengthChanged    (   int32   iValue  )
{
    paintEngine.SetSmoothingStrength( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleSmoothingEnabledChanged     (   bool    iValue  )
{
    paintEngine.SetSmoothingEnabled( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleSmoothingRealTimeChanged    (   bool    iValue  )
{
    paintEngine.SetSmoothingRealTime( iValue );
}


void
FOdysseyPainterEditorToolkit::HandleSmoothingCatchUpChanged     (   bool    iValue  )
{
    paintEngine.SetSmoothingCatchUp( iValue );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Performance Handlers
void
FOdysseyPainterEditorToolkit::HandlePerformanceLiveUpdateChanged(   bool    iValue  )
{
    liveUpdateInfo.enabled = iValue;
    displaySurface->Invalidate();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks
TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushSelector( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == BrushSelectorTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "BrushSelectorTitle", "BrushSelector" ) )
        [
            SNew( SBox )
            .HeightOverride( 50 )
            [
                BrushSelectorTab.ToSharedRef()
            ]
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == MeshSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "MeshSelectorTitle", "MeshSelector" ) )
        [
            MeshSelectorTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == BrushExposedParametersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "BrushExposedParametersTitle", "Brush Exposed Parameters" ) )
        [
            BrushExposedParametersTab.ToSharedRef()
        ];
}

TSharedRef<SDockTab>
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == LayerStackTabId);

    return SNew(SDockTab)
        .Label(LOCTEXT("LayerStackTitle", "LayerStack"))
        [
            LayerStackTab.ToSharedRef()
        ];

}


TSharedRef<SDockTab>
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == ViewportTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "ViewportTabTitle", "Viewport" ) )
        [
            ViewportTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSelector( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == ColorSelectorTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "ColorSelectorTitle", "Color Selector" ) )
        [
            ColorSelectorTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnColorSliders( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == ColorSlidersTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "ColorSlidersTitle", "Color Sliders" ) )
        [
            ColorSlidersTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnBotBar( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == BotBarTabId );

    return  SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "BotBarTitle", "BotBar" ) )
        [
            SNew( SBox )
            .HeightOverride( 20 )
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTopBar( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == TopBarTabId );

    return  SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "TopBarTitle", "TopBar" ) )
        [
            TopTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnStrokeOptions( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == StrokeOptionsTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "StrokeOptionsTitle", "Stroke Options" ) )
        [
            StrokeOptionsTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnPerformanceOptions( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == PerformanceOptionsTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "PerformanceOptionsTitle", "Performance Options" ) )
        [
            PerformanceOptionsTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnNotes( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == NotesTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "Notes Title", "Notes" ) )
        [
            SNew( SMultiLineEditableText )
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnUndoHistory( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == UndoHistoryTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "Undo History Title", "Undo History" ) )
        [
            UndoHistoryTab.ToSharedRef()
        ];
}


TSharedRef< SDockTab >
FOdysseyPainterEditorToolkit::HandleTabSpawnerSpawnTools( const FSpawnTabArgs& Args )
{
    check( Args.GetTabId() == ToolsTabId );

    return  SNew( SDockTab )
        .Label( LOCTEXT( "Tool Box", "Tools" ) )
        [
            SNew(SScrollBox)
            .Orientation(Orient_Vertical)
            .ScrollBarAlwaysVisible(false)
            +SScrollBox::Slot()
            [
                SNew( SExpandableArea)
                .HeaderContent()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("Utils", "Utils"))
                    .Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                    .ShadowOffset(FVector2D(1.0f, 1.0f))
                ]
                .BodyContent()
                [
                    SNew(SWrapBox)
                    .UseAllottedWidth(true)
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnClearCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Shredder32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        .OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.PaintBucket32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.ColorPicker32"))
                        ]
                    ]
                ]
            ]
            +SScrollBox::Slot()
            [
                SNew( SExpandableArea)
                .HeaderContent()
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("Shapes", "Shapes"))
                    .Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                    .ShadowOffset(FVector2D(1.0f, 1.0f))
                ]
                .BodyContent()
                [
                    SNew(SWrapBox)
                    .UseAllottedWidth(true)
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Line32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Square32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Circle32"))
                        ]
                    ]
                        +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Ellipse32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Curve32"))
                        ]
                    ]
                    +SWrapBox::Slot()
                    [
                        SNew( SButton )
                        .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                        //.OnClicked(this, &FOdysseyPainterEditorToolkit::OnFillCurrentLayer)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Bezier32"))
                        ]
                    ]
                ]
            ]
        ];
}

FReply
FOdysseyPainterEditorToolkit::OnClearCurrentLayer()
{
    paintEngine.AbortStroke();
    layer_stack.ClearCurrentLayer();
    if( bLiveUpdateEnabled )
        InvalidateTextureFromData( layer_stack.GetResultBlock(), texture );
    return FReply::Handled();
}

FReply
FOdysseyPainterEditorToolkit::OnFillCurrentLayer()
{
    paintEngine.AbortStroke();
    layer_stack.FillCurrentLayerWithColor( paintEngine.GetColor() );
    return FReply::Handled();
}


void
FOdysseyPainterEditorToolkit::SetColor( const ::ULIS::CColor& iColor )
{
    if( ColorSelectorTab ) ColorSelectorTab->SetColor( iColor );
}



#undef LOCTEXT_NAMESPACE

