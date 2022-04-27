// IDDN.FR.001.220036.001.S.P.2021.000.00000
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
#include "ToolkitHelpers.h"
#include "Misc/EposSequenceEditorPlaybackContext.h"
#include "Render/EposSequencePipelineRenderer.h"

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
    // register sequencer menu extenders
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    int32 NewIndex = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().Add(
        FAssetEditorExtender::CreateRaw( this, &FEposSequenceEditorToolkit::HandleMenuExtensibilityGetExtender ) );
    mSequencerExtenderHandle = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates()[NewIndex].GetHandle();

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

    // unregister sequencer menu extenders
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().RemoveAll( [this]( const FAssetEditorExtender& Extender )
    {
        return mSequencerExtenderHandle == Extender.GetHandle();
    } );
}

void FEposSequenceEditorToolkit::Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, TArray< UEposMovieSceneSequence* > iSequences )
{
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

    mSequencer->OnActorAddedToSequencer().AddSP( this, &FEposSequenceEditorToolkit::HandleActorAddedToSequencer );
    mSequencer->OnActivateSequence().AddSP( this, &FEposSequenceEditorToolkit::HandleOnActivateSequence );
    mSequencer->GetSelectionChangedSections().AddSP( this, &FEposSequenceEditorToolkit::HandleOnSelectionChangedSections );

    // Force the epos renderer when opening a board asset
    // And there is a bug in the render movie popup display, all renderers are checked, but it's only ui
    mSequencer->GetSequencerSettings()->SetMovieRendererName( FEposSequencePipelineRenderer::MoviePipelineQueueTabLabel.ToString() );

    //if( ToolkitParams.InitialBindingClass != nullptr )
    //{
    //    FTemplateSequenceEditorUtil Util( BoardSequence, *Sequencer.Get() );
    //    Util.ChangeActorBinding( ToolkitParams.InitialBindingClass );
    //}

    // with ToolkitCommands, it's for shortcuts only
    BindCommands( mSequencer->GetCommandBindings() );

    FLevelEditorSequencerIntegrationOptions options;
    options.bRequiresLevelEvents = true;
    options.bRequiresActorEvents = true;
    FLevelEditorSequencerIntegration::Get().AddSequencer( mSequencer.ToSharedRef(), options );

    FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );

    // Reopen the scene outliner so that is refreshed with the sequencer columns
    {
        TSharedPtr<FTabManager> levelEditorTabManager = levelEditorModule.GetLevelEditorTabManager();
        if( levelEditorTabManager->FindExistingLiveTab( FName( "LevelEditorSceneOutliner" ) ).IsValid() ) // SceneOutliner == WorldOutliner ...
        {
            levelEditorTabManager->TryInvokeTab( FName( "LevelEditorSceneOutliner" ) )->RequestCloseTab();
            levelEditorTabManager->TryInvokeTab( FName( "LevelEditorSceneOutliner" ) );
        }
    }

    levelEditorModule.AttachSequencer( mSequencer->GetSequencerWidget(), SharedThis( this ) );
    levelEditorModule.OnMapChanged().AddRaw( this, &FEposSequenceEditorToolkit::HandleMapChanged );

    OnOpened().Broadcast( *this );

    //---

    auto GetFirstPerspectiveClient = []() -> SLevelViewport*
    {
        FLevelEditorViewportClient* levelVC = nullptr;

        for( FLevelEditorViewportClient* viewportClient : GEditor->GetLevelViewportClients() )
        {
            if( viewportClient
                && viewportClient->GetViewMode() != VMI_Unknown
                && viewportClient->AllowsCinematicControl()
                && viewportClient->IsPerspective() )
                //TODO: improve by getting an already "storyboard viewport" if exists
            {
                levelVC = viewportClient;
                break;
            }
        }

        if( !levelVC )
        {
            //TODO: improve by setting to perspective if no one find
            //viewport->GetLevelViewportClient().SetViewportType(ELevelViewportType::LVT_Perspective); // Need to be called first

            FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
            return levelEditorModule.GetFirstActiveLevelViewport().Get();
        }

        TSharedPtr<SEditorViewport> viewport_widget = levelVC->GetEditorViewportWidget();
        return StaticCastSharedPtr<SLevelViewport>( viewport_widget ).Get();
    };

    // Do not use a TSharedPtr<> !!!
    // https://udn.unrealengine.com/s/question/0D54z00007bITs8CAG/changing-the-viewport-layout-type-makes-the-viewport-unfocused-and-gcurrentleveleditingviewportclient-nullptr
    SLevelViewport* viewport = GetFirstPerspectiveClient();
    if( viewport )
    {
        viewport->GetCommandList()->ExecuteAction( FEposSequenceEditorCommands::Get().ToggleStoryboardViewportCommand.ToSharedRef() );
        viewport = nullptr; // viewport is no more valid after changing viewport layout type

        // Only for 5.0.1
        viewport = GetFirstPerspectiveClient();
        viewport->GetLevelViewportClient().SetCurrentViewport();
    }
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

        UMovieSceneTrack* track = sequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
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
    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoPraxinos,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::GotoPraxinos )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoForum,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::GotoForum )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoDiscord,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::GotoDiscord )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoUserDocumentation,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::GotoUserDocumentation )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().GotoProjects,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::GotoProjects )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenAboutWindow,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::OpenAboutWindow )
    );

    //---

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenSequenceEditorSettings,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::OpenSequenceEditorSettings )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenTrackEditorSettings,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::OpenTrackEditorSettings )
    );

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().OpenNamingConventionEditorSettings,
        FExecuteAction::CreateStatic( &FEposSequenceEditorActionCallbacks::OpenNamingConventionEditorSettings )
    );

    TSharedPtr< ILevelEditor > levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor").GetFirstLevelEditor();
    levelEditor->AppendCommands( CommandList.ToSharedRef() );
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

//---

TSharedRef<FExtender>
FEposSequenceEditorToolkit::HandleMenuExtensibilityGetExtender( const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects )
{
    TSharedRef<FExtender> AddTrackMenuExtender( new FExtender() );
    AddTrackMenuExtender->AddMenuExtension(
        SequencerMenuExtensionPoints::AddTrackMenu_PropertiesSection,
        EExtensionHook::Before,
        CommandList,
        FMenuExtensionDelegate::CreateRaw( this, &FEposSequenceEditorToolkit::HandleTrackMenuExtensionAddTrack, ContextSensitiveObjects ) );

    return AddTrackMenuExtender;
}

void
FEposSequenceEditorToolkit::HandleTrackMenuExtensionAddTrack( FMenuBuilder& AddTrackMenuBuilder, TArray<UObject*> ContextObjects )
{
    // TODO-lchabant: stolen from level sequence.
    if( ContextObjects.Num() != 1 )
    {
        return;
    }

    AActor* Actor = Cast<AActor>( ContextObjects[0] );
    if( Actor == nullptr )
    {
        return;
    }

    AddTrackMenuBuilder.BeginSection( "Components", LOCTEXT( "ComponentsSection", "Components" ) );
    {
        TMap<FString, UActorComponent*> SortedComponents;
        for( UActorComponent* Component : Actor->GetComponents() )
        {
            if( Component )
            {
                SortedComponents.Add( Component->GetName(), Component );
            }
        }
        SortedComponents.KeySort( []( const FString& A, const FString& B )
                                  {
                                      return A < B;
                                  } );

        for( const TPair<FString, UActorComponent*>& Component : SortedComponents )
        {
            FUIAction AddComponentAction( FExecuteAction::CreateSP( this, &FEposSequenceEditorToolkit::HandleAddComponentActionExecute, Component.Value ) );
            FText AddComponentLabel = FText::FromString( Component.Key );
            FText AddComponentToolTip = FText::Format( LOCTEXT( "ComponentToolTipFormat", "Add {0} component" ), AddComponentLabel );
            AddTrackMenuBuilder.AddMenuEntry( AddComponentLabel, AddComponentToolTip, FSlateIcon(), AddComponentAction );
        }
    }
    AddTrackMenuBuilder.EndSection();
}

void
FEposSequenceEditorToolkit::HandleAddComponentActionExecute( UActorComponent* Component )
{
    // TODO-lchabant: stolen from level sequence.
    const FScopedTransaction Transaction( LOCTEXT( "AddComponent", "Add Component" ) );

    FString ComponentName = Component->GetName();

    TArray<UActorComponent*> ActorComponents;
    ActorComponents.Add( Component );

    USelection* SelectedActors = GEditor->GetSelectedActors();
    if( SelectedActors && SelectedActors->Num() > 0 )
    {
        for( FSelectionIterator Iter( *SelectedActors ); Iter; ++Iter )
        {
            AActor* Actor = CastChecked<AActor>( *Iter );

            TArray<UActorComponent*> OutActorComponents;
            Actor->GetComponents( OutActorComponents );

            for( UActorComponent* ActorComponent : OutActorComponents )
            {
                if( ActorComponent->GetName() == ComponentName )
                {
                    ActorComponents.AddUnique( ActorComponent );
                }
            }
        }
    }

    for( UActorComponent* ActorComponent : ActorComponents )
    {
        mSequencer->GetHandleToObject( ActorComponent );
    }
}

//---

void FEposSequenceEditorToolkit::HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding )
{
    ToolkitHelpers::CreateDefaultTracksForActor( mSequencer.Get(), iActor, iBinding );

    ToolkitHelpers::FixCameraBindingOnCameraCut( mSequencer.Get(), iActor, iBinding );

    //PATCH: replace standard cameracut track (if exists) by our single cameracut track
    ToolkitHelpers::PatchStandardCameraCutTrack( mSequencer.Get(), iActor, iBinding );
}

void FEposSequenceEditorToolkit::HandleOnActivateSequence( FMovieSceneSequenceIDRef iSequenceID )
{
    check( iSequenceID == mSequencer->GetFocusedTemplateID() );

    auto playback_range = mSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange();
    FQualifiedFrameTime time = mSequencer->GetLocalTime();

    if( playback_range.Contains( time.Time.GetFrame() ) )
        return;

    mSequencer->SetLocalTime( playback_range.GetLowerBoundValue() );
}

void FEposSequenceEditorToolkit::HandleOnSelectionChangedSections( TArray<UMovieSceneSection*> iSections )
{
    if( !iSections.Num() )
        return;

    //PATCH: sometimes the given section is not inside the current sequence
    // first down (of the double click) on a section in the root sequence
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : local time 102000
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : 1 sections
    //     [2021.07.27 - 08.40.02:647][497]LogTemp : Warning : GetSelectionChangedSections : xxxboard0010_01xxx
    // unselect the section before changing the focused sequence
    //     [2021.07.27 - 08.40.02:715][505]LogTemp : Warning : GetSelectionChangedSections : local time 0
    //     [2021.07.27 - 08.40.02:716][505]LogTemp : Warning : GetSelectionChangedSections : 0 sections
    // change the focused sequence to make the subsequence the focused one
    //     [2021.07.27 - 08.40.02:716][505]LogTemp : Warning : OnActivateSequence
    // !!!
    // the section inside the root sequence (previous focused sequence) is still set as selected (but not everytime)
    // and this makes the "set local time" wrong due to the lower bound value of the section which is related to the root sequence and not the new focused one
    // !!!
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : local time 0
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : 1 sections
    //     [2021.07.27 - 08.40.02:793][514]LogTemp : Warning : GetSelectionChangedSections : xxxboard0010_01xxx
    // so we check the given section is one of the section in the focused sequence
    if( !mSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetAllSections().Contains( iSections.Last() ) )
        return;

    FQualifiedFrameTime time = mSequencer->GetLocalTime();
    if( iSections.Last()->GetTrueRange().Contains( time.Time.GetFrame() ) )
        return;

    mSequencer->SetLocalTime( iSections.Last()->GetTrueRange().GetLowerBoundValue() );
}

void FEposSequenceEditorToolkit::HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType )
{
    if( ( iMapChangeType == EMapChangeType::LoadMap || iMapChangeType == EMapChangeType::NewMap || iMapChangeType == EMapChangeType::TearDownWorld ) )
    {
        mSequencer->GetSpawnRegister().CleanUp( *mSequencer );
        CloseWindow();
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
