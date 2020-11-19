// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceEditorCommands.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Misc/EposSequenceEditorPlaybackContext.h"
#include "Shot/ShotSequenceEditorCommands.h"
#include "ShotHelpers/ShotSequenceHelpers.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorToolkit"

const FName FEposSequenceEditorToolkit::smSequencerMainTabId( TEXT( "Sequencer_SequencerMain" ) );

namespace SequencerDefs
{
    static const FName sgEposSequencerAppIdentifier( TEXT( "EposSequencerApp" ) );
}

FEposSequenceEditorToolkit::FEposSequenceEditorToolkit( const TSharedRef<ISlateStyle>& iStyle )
    : mSequence( nullptr )
    , mStyle( iStyle )
{
    // register sequencer menu extenders
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    int32 NewIndex = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().Add(
        FAssetEditorExtender::CreateRaw( this, &FEposSequenceEditorToolkit::HandleMenuExtensibilityGetExtender ) );
    mSequencerExtenderHandle = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates()[NewIndex].GetHandle();
}

FEposSequenceEditorToolkit::~FEposSequenceEditorToolkit()
{
    FLevelEditorSequencerIntegration::Get().RemoveSequencer( mSequencer.ToSharedRef() );

    mSequencer->Close();

    // unregister delegates
    if( FModuleManager::Get().IsModuleLoaded( TEXT( "LevelEditor" ) ) )
    {
        auto& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
        levelEditorModule.OnMapChanged().RemoveAll( this );
    }

    // unregister sequencer menu extenders
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().RemoveAll( [this]( const FAssetEditorExtender& Extender )
    {
        return mSequencerExtenderHandle == Extender.GetHandle();
    } );
}

void FEposSequenceEditorToolkit::Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, TArray< UMovieSceneSequence* > iSequences )
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
    mPlaybackContext = MakeShared<FEposSequenceEditorPlaybackContext>();

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

        sequencerInitParams.PlaybackContext.Bind( mPlaybackContext.ToSharedRef(), &FEposSequenceEditorPlaybackContext::GetPlaybackContext );

        sequencerInitParams.ViewParams.UniqueName = "EposSequencerEditor";
        sequencerInitParams.ViewParams.ScrubberStyle = ESequencerScrubberStyle::FrameBlock;
        sequencerInitParams.ViewParams.OnReceivedFocus.BindRaw( this, &FEposSequenceEditorToolkit::OnSequencerReceivedFocus );
    }

    mSequencer = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" ).CreateSequencer( sequencerInitParams );

    GoToFocusedSequence( iSequences );

    mSequencer->OnActorAddedToSequencer().AddSP( this, &FEposSequenceEditorToolkit::HandleActorAddedToSequencer );

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
    options.bCanRecord = true;
    FLevelEditorSequencerIntegration::Get().AddSequencer( mSequencer.ToSharedRef(), options );

    FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );

    // Reopen the scene outliner so that is refreshed with the sequencer info column
    if( mSequencer->GetSequencerSettings()->GetShowOutlinerInfoColumn() )
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
}

void FEposSequenceEditorToolkit::GoToFocusedSequence( TArray< UMovieSceneSequence* > iSequences )
{
    check( mSequencer );
    check( iSequences.Num() );

    for( int i = 0; i < iSequences.Num() - 1; i++ )
    {
        UBoardSequence* sequence = Cast<UBoardSequence>( iSequences[i] );
        if( !sequence )
            continue;
        UMovieSceneSequence* child_sequence = iSequences[i + 1]; // May be a Board or Shot sequence
        if( !child_sequence )
            continue;

        UMovieSceneTrack* track = sequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( !track )
            continue;

        for( auto section : track->GetAllSections() )
        {
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            UMovieSceneSequence* sub_sequence = board_section->GetSequence();
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
}

//--- FGCObject interface

void
FEposSequenceEditorToolkit::AddReferencedObjects( FReferenceCollector& iCollector )
{
    if( mSequence )
        iCollector.AddReferencedObject( mSequence );
}

//--- FAssetEditorToolkit interface

bool FEposSequenceEditorToolkit::OnRequestClose()
{
    return true;
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

void FEposSequenceEditorToolkit::RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    if( IsWorldCentricAssetEditor() )
    {
        return;
    }

    checkf( false, TEXT( "should never go here as it should always be world-centric" ) );
}

void FEposSequenceEditorToolkit::UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    if( !IsWorldCentricAssetEditor() )
    {
        checkf( false, TEXT( "should never go here as it should always be world-centric" ) );
    }

    FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    levelEditorModule.AttachSequencer( SNullWidget::NullWidget, nullptr );
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
        for( UActorComponent* Component : Actor->GetComponents() )
        {
            if( Component )
            {
                FUIAction AddComponentAction( FExecuteAction::CreateSP( this, &FEposSequenceEditorToolkit::HandleAddComponentActionExecute, Component ) );
                FText AddComponentLabel = FText::FromString( Component->GetName() );
                FText AddComponentToolTip = FText::Format( LOCTEXT( "ComponentToolTipFormat", "Add {0} component" ), FText::FromString( Component->GetName() ) );
                AddTrackMenuBuilder.AddMenuEntry( AddComponentLabel, AddComponentToolTip, FSlateIcon(), AddComponentAction );
            }
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
    ShotSequenceHelpers::CreateDefaultTracksForActor( mSequencer.Get(), iActor, iBinding );

    ShotSequenceHelpers::FixCameraBindingOnCameraCut( mSequencer.Get(), iActor, iBinding );

    //PATCH: replace standard cameracut track (if exists) by our single cameracut track
    ShotSequenceHelpers::PatchStandardCameraCutTrack( mSequencer.Get(), iActor, iBinding );
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
