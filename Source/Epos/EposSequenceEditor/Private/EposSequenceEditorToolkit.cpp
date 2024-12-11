// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceEditorToolkit.h"

#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "DragAndDrop/ActorDragDropGraphEdOp.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "DragAndDrop/ClassDragDropOp.h"
#include "Engine/Selection.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ISequencer.h"
#include "ISequencerModule.h"
#include "LevelEditor.h"
#include "LevelEditorSequencerIntegration.h"
//#include "Misc/BoardSequenceEditorPlaybackContext.h"
//#include "Misc/TemplateSequenceEditorSpawnRegister.h"
//#include "Misc/TemplateSequenceEditorUtil.h"
#include "Modules/ModuleManager.h"
#include "MovieSceneSequence.h"
#include "ScopedTransaction.h"
#include "SequencerSettings.h"
#include "SLevelViewport.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "EposSequenceEditorCommands.h"
#include "Export/ExportSequencerRenderer.h"
#include "Misc/EposSequenceEditorPlaybackContext.h"
#include "Render/EposSequencePipelineRenderer.h"
#include "ToolkitHelpers.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorToolkit"

//---

const FName FEposSequenceEditorToolkit::smSequencerMainTabId( TEXT( "Sequencer_SequencerMain" ) );

namespace SequencerDefs
{
    static const FName sgEposSequencerAppIdentifier( TEXT( "EposSequencerApp" ) );
}

//---

static TArray<FEposSequenceEditorToolkit*> sgOpenToolkits;

void
FEposSequenceEditorToolkit::IterateOpenToolkits(TFunctionRef<bool(FEposSequenceEditorToolkit&)> Iter)
{
    for( FEposSequenceEditorToolkit* Toolkit : sgOpenToolkits )
    {
        if (!Iter(*Toolkit))
        {
            return;
        }
    }
}

FEposSequenceEditorToolkit::FEposSequenceEditorToolkitOpened&
FEposSequenceEditorToolkit::OnOpened()
{
    static FEposSequenceEditorToolkitOpened sOnOpenedEvent;
    return sOnOpenedEvent;
}

FEposSequenceEditorToolkit::FEposSequenceEditorToolkitClosed&
FEposSequenceEditorToolkit::OnClosed()
{
    return mOnClosedEvent;
}

//---

FEposSequenceEditorToolkit::FEposSequenceEditorToolkit()
    : mSequence( nullptr )
{
    sgOpenToolkits.Add( this );
}

FEposSequenceEditorToolkit::~FEposSequenceEditorToolkit()
{
    if( FModuleManager::Get().IsModuleLoaded( TEXT( "LevelEditor" ) ) )
    {
        FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

        // @todo remove when world-centric mode is added
        levelEditorModule.AttachSequencer( SNullWidget::NullWidget, nullptr );
        FLevelEditorSequencerIntegration::Get().RemoveSequencer( mSequencer.ToSharedRef() );

        // unregister delegates
        levelEditorModule.OnMapChanged().RemoveAll( this );
    }

    mSequencer->Close();
}

void FEposSequenceEditorToolkit::Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, TArray< UEposMovieSceneSequence* > iSequences )
{
    FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );

    // Clear out the existing sequencer
    levelEditorModule.AttachSequencer( nullptr, nullptr );

    // create tab layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout( "Standalone_EposEditor" )
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab( smSequencerMainTabId, ETabState::OpenedTab )
            )
        );

    mSequence = iSequences[0]; // =Root
    mPlaybackContext = MakeShared<FEposSequenceEditorPlaybackContext>( mSequence );

    // Mode sould always be world-centric (don't know how to have a standalone one)
    // in this case, SequencerDefs::ShotSequencerAppIdentifier & StandaloneDefaultLayout is not useful
    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = false;
    FAssetEditorToolkit::InitAssetEditor( iMode, iInitToolkitHost, SequencerDefs::sgEposSequencerAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, mSequence );

    //TSharedRef<FTemplateSequenceEditorSpawnRegister> SpawnRegister = MakeShareable( new FTemplateSequenceEditorSpawnRegister() );
    //SpawnRegister->SetSequencer( Sequencer );

    // Initialize sequencer.
    FSequencerInitParams sequencerInitParams;
    {
        sequencerInitParams.RootSequence = mSequence;
        sequencerInitParams.bEditWithinLevelEditor = true;
        sequencerInitParams.ToolkitHost = iInitToolkitHost;
        //sequencerInitParams.SpawnRegister = SpawnRegister;
        sequencerInitParams.HostCapabilities.bSupportsCurveEditor = true;
        sequencerInitParams.HostCapabilities.bSupportsSaveMovieSceneAsset = true;
        //sequencerInitParams.HostCapabilities.bSupportsRecording = true;
        sequencerInitParams.HostCapabilities.bSupportsRenderMovie = true;

        //sequencerInitParams.EventContexts.Bind( PlaybackContext.ToSharedRef(), &FLevelSequencePlaybackContext::GetEventContexts );
        sequencerInitParams.PlaybackContext.Bind( mPlaybackContext.ToSharedRef(), &FEposSequenceEditorPlaybackContext::GetPlaybackContextAsObject );
        //sequencerInitParams.PlaybackClient.Bind( mPlaybackContext.ToSharedRef(), &FEposSequenceEditorPlaybackContext::GetPlaybackClientAsInterface );

        sequencerInitParams.ViewParams.UniqueName = "EposSequencerEditor";
        sequencerInitParams.ViewParams.ScrubberStyle = ESequencerScrubberStyle::FrameBlock;
        sequencerInitParams.ViewParams.OnReceivedFocus.BindRaw( this, &FEposSequenceEditorToolkit::OnSequencerReceivedFocus );
    }

    mSequencer = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" ).CreateSequencer( sequencerInitParams );

    GoToFocusedSequence( iSequences );

    // Force the epos renderer when opening a board asset
    // And there is a bug in the render movie popup display, all renderers are checked, but it's only ui
    FString movie_render_name = mSequencer->GetSequencerSettings()->GetMovieRendererName();
    if( movie_render_name != FEposSequencePipelineRenderer::MoviePipelineQueueTabLabel.ToString()
        && movie_render_name != FExportSequencerRenderer::ExportTabLabel.ToString() )
    {
        mSequencer->GetSequencerSettings()->SetMovieRendererName( FEposSequencePipelineRenderer::MoviePipelineQueueTabLabel.ToString() );
    }

    // with ToolkitCommands, it's for shortcuts only
    BindCommands( mSequencer->GetCommandBindings() );

    FLevelEditorSequencerIntegrationOptions options;
    options.bRequiresLevelEvents = true;
    options.bRequiresActorEvents = true;
    FLevelEditorSequencerIntegration::Get().AddSequencer( mSequencer.ToSharedRef(), options );

    // Reopen the scene outliner so that is refreshed with the sequencer columns
    {
        TSharedPtr<FTabManager> levelEditorTabManager = levelEditorModule.GetLevelEditorTabManager();
        if( levelEditorTabManager->FindExistingLiveTab( FName( "LevelEditorSceneOutliner" ) ).IsValid() ) // SceneOutliner == WorldOutliner ...
        {
            levelEditorTabManager->TryInvokeTab( FName( "LevelEditorSceneOutliner" ) )->RequestCloseTab();
            levelEditorTabManager->TryInvokeTab( FName( "LevelEditorSceneOutliner" ) );
        }
    }

    TSharedPtr<SDockTab> dockTab = levelEditorModule.AttachSequencer( mSequencer->GetSequencerWidget(), SharedThis( this ) );
    if( dockTab.IsValid() )
    {
        TAttribute<FText> labelSuffix = TAttribute<FText>( this, &FEposSequenceEditorToolkit::GetTabSuffix );
        dockTab->SetTabLabelSuffix( labelSuffix );
    }

    levelEditorModule.OnMapChanged().AddRaw( this, &FEposSequenceEditorToolkit::HandleMapChanged );

    OnOpened().Broadcast( *this );

    //---

    ToolkitHelpers::SetStoryboardViewport();
}

TSharedPtr<ISequencer>
FEposSequenceEditorToolkit::GetSequencer() const //override
{
    return mSequencer;
}

void FEposSequenceEditorToolkit::GoToFocusedSequence( TArray< UEposMovieSceneSequence* > iSequences )
{
    check( mSequencer );
    check( iSequences.Num() );

    for( int i = 0; i < iSequences.Num() - 1; i++ )
    {
        UBoardSequence* sequence = Cast<UBoardSequence>( iSequences[i] );
        if( !sequence )
            continue;
        UEposMovieSceneSequence* child_sequence = iSequences[i + 1]; // May be a Board or Shot sequence
        if( !child_sequence )
            continue;

        UMovieSceneTrack* track = sequence->GetMovieScene()->FindTrack<UMovieSceneCinematicBoardTrack>();
        if( !track )
            continue;

        for( auto section : track->GetAllSections() )
        {
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            UEposMovieSceneSequence* sub_sequence = Cast<UEposMovieSceneSequence>( board_section->GetSequence() );
            if( !sub_sequence )
                continue;

            if( sub_sequence->GetFullName() == child_sequence->GetFullName() )
                mSequencer->FocusSequenceInstance( *board_section );
        }
    }
}

void
FEposSequenceEditorToolkit::BindCommands( TSharedPtr<FUICommandList> CommandList )
{
    // Adding commands here is certainly not a good idea, as they won't be created when this toolkit is not called
    // For example, opening a level sequence (level sequence toolkit will be created) won't create this toolkit at all
}

//--- FGCObject interface

void
FEposSequenceEditorToolkit::AddReferencedObjects( FReferenceCollector& iCollector )
{
    if( mSequence )
        iCollector.AddReferencedObject( mSequence );
}

FString
FEposSequenceEditorToolkit::GetReferencerName() const //override
{
    return "FEposSequenceEditorToolkit";
}

//--- FAssetEditorToolkit interface

void FEposSequenceEditorToolkit::OnClose()
{
    sgOpenToolkits.Remove( this );

    mOnClosedEvent.Broadcast();
}

bool FEposSequenceEditorToolkit::CanFindInContentBrowser() const
{
    // False so that sequencer doesn't take over Find In Content Browser functionality and always find the level sequence asset.
    return false;
}

//--- IToolkit interface

FText FEposSequenceEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Epos Sequence Editor" );
}

FName FEposSequenceEditorToolkit::GetToolkitFName() const
{
    static FName sSequencerName( "EposSequenceEditor" );
    return sSequencerName;
}

FString FEposSequenceEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Sequencer " ).ToString();
}

FLinearColor FEposSequenceEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.7, 0.0f, 0.2f, 0.5f );
}

FText FEposSequenceEditorToolkit::GetTabSuffix() const
{
    UMovieSceneSequence* sequence = mSequencer->GetFocusedMovieSceneSequence();

    if( sequence == nullptr )
    {
        return FText::GetEmpty();
    }

    const bool bIsDirty = sequence->GetMovieScene()->GetOuter()->GetOutermost()->IsDirty();
    if( bIsDirty )
    {
        return LOCTEXT( "TabSuffixAsterix", "*" );
    }

    return FText::GetEmpty();
}

void FEposSequenceEditorToolkit::BringToolkitToFront()
{
    IEposSequenceEditorToolkit::BringToolkitToFront();

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
    LevelEditorTabManager->TryInvokeTab( LevelEditorTabIds::Sequencer );
}

//---

void FEposSequenceEditorToolkit::HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType )
{
    if( ( iMapChangeType == EMapChangeType::LoadMap || iMapChangeType == EMapChangeType::NewMap || iMapChangeType == EMapChangeType::TearDownWorld ) )
    {
        mSequencer->GetSpawnRegister().CleanUp( *mSequencer );
        CloseWindow( EAssetEditorCloseReason::AssetUnloadingOrInvalid );
    }
}

void FEposSequenceEditorToolkit::OnSequencerReceivedFocus()
{
    if( mSequencer.IsValid() )
    {
        FLevelEditorSequencerIntegration::Get().OnSequencerReceivedFocus( mSequencer.ToSharedRef() );
    }
}

#undef LOCTEXT_NAMESPACE
