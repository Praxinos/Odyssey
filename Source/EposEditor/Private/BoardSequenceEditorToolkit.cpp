// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardSequenceEditorToolkit.h"

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
#include "ScopedTransaction.h"
#include "SequencerSettings.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#include "BoardSequenceEditorCommands.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Misc/EposEditorPlaybackContext.h"

#define LOCTEXT_NAMESPACE "BoardSequenceEditor"

const FName FBoardSequenceEditorToolkit::smSequencerMainTabId( TEXT( "Sequencer_SequencerMain" ) );

namespace SequencerDefs
{
    static const FName sgBoardSequencerAppIdentifier( TEXT( "BoardSequencerApp" ) );
}

FBoardSequenceEditorToolkit::FBoardSequenceEditorToolkit( const TSharedRef<ISlateStyle>& iStyle )
    : mBoardSequence( nullptr )
    , mStyle( iStyle )
{
    //ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    //int32 NewIndex = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().Add(
    //    FAssetEditorExtender::CreateRaw( this, &FTemplateSequenceEditorToolkit::HandleMenuExtensibilityGetExtender ) );
    //SequencerExtenderHandle = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates()[NewIndex].GetHandle();
}

FBoardSequenceEditorToolkit::~FBoardSequenceEditorToolkit()
{
    FLevelEditorSequencerIntegration::Get().RemoveSequencer( mSequencer.ToSharedRef() );

    mSequencer->Close();

    if( FModuleManager::Get().IsModuleLoaded( TEXT( "LevelEditor" ) ) )
    {
        auto& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
        levelEditorModule.OnMapChanged().RemoveAll( this );
    }

    //ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    //SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().RemoveAll( [this]( const FAssetEditorExtender& Extender )
    //{
    //    return SequencerExtenderHandle == Extender.GetHandle();
    //} );
}

void FBoardSequenceEditorToolkit::Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, UBoardSequence* iBoardSequence )
{
    // create tab layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout( "Standalone_BoardSequenceEditor" )
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab( smSequencerMainTabId, ETabState::OpenedTab )
            )
        );

    mBoardSequence = iBoardSequence;
    mPlaybackContext = MakeShared<FEposEditorPlaybackContext>();

    // Mode sould always be world-centric (don't know how to have a standalone one)
    // in this case, SequencerDefs::ShotSequencerAppIdentifier & StandaloneDefaultLayout is not useful
    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = false;
    FAssetEditorToolkit::InitAssetEditor( iMode, iInitToolkitHost, SequencerDefs::sgBoardSequencerAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, mBoardSequence );

    //TSharedRef<FTemplateSequenceEditorSpawnRegister> SpawnRegister = MakeShareable( new FTemplateSequenceEditorSpawnRegister() );
    //SpawnRegister->SetSequencer( Sequencer );

    // Initialize sequencer.
    FSequencerInitParams sequencerInitParams;
    {
        sequencerInitParams.RootSequence = mBoardSequence;
        sequencerInitParams.bEditWithinLevelEditor = true;
        sequencerInitParams.ToolkitHost = iInitToolkitHost;
        //sequencerInitParams.SpawnRegister = SpawnRegister;
        sequencerInitParams.HostCapabilities.bSupportsCurveEditor = true;
        sequencerInitParams.HostCapabilities.bSupportsSaveMovieSceneAsset = true;

        sequencerInitParams.PlaybackContext.Bind( mPlaybackContext.ToSharedRef(), &FEposEditorPlaybackContext::GetPlaybackContext );

        sequencerInitParams.ViewParams.UniqueName = "EposSequenceEditor";
        sequencerInitParams.ViewParams.ScrubberStyle = ESequencerScrubberStyle::FrameBlock;
        sequencerInitParams.ViewParams.OnReceivedFocus.BindRaw( this, &FBoardSequenceEditorToolkit::OnSequencerReceivedFocus );
    }

    mSequencer = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" ).CreateSequencer( sequencerInitParams );

    mSequencer->OnActorAddedToSequencer().AddSP( this, &FBoardSequenceEditorToolkit::HandleActorAddedToSequencer );

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
    levelEditorModule.OnMapChanged().AddRaw( this, &FBoardSequenceEditorToolkit::HandleMapChanged );
}

void
FBoardSequenceEditorToolkit::BindCommands( TSharedPtr<FUICommandList> CommandList )
{
    const UEposEditorSettings* settings = GetDefault<UEposEditorSettings>();

    CommandList->MapAction(
        FBoardSequenceEditorCommands::Get().ArrangeShots,
        FExecuteAction::CreateSP( this, &FBoardSequenceEditorToolkit::HandleArrangeShots )
    );
    CommandList->MapAction(
        FBoardSequenceEditorCommands::Get().ArrangeShotsOnOneRow,
        FExecuteAction::CreateSP( this, &FBoardSequenceEditorToolkit::HandleArrangeShots, EArrangeShots::OnOneRow ),
        FCanExecuteAction::CreateLambda([this] { return true; }),
        FIsActionChecked::CreateLambda([] { return GetDefault<UEposEditorSettings>()->BoardTrackSettings.ArrangeShots == EArrangeShots::OnOneRow; } )
    );
    CommandList->MapAction(
        FBoardSequenceEditorCommands::Get().ArrangeShotsOnTwoRows,
        FExecuteAction::CreateSP( this, &FBoardSequenceEditorToolkit::HandleArrangeShots, EArrangeShots::OnTwoRowsShifted ),
        FCanExecuteAction::CreateLambda([this] { return true; }),
        FIsActionChecked::CreateLambda([] { return GetDefault<UEposEditorSettings>()->BoardTrackSettings.ArrangeShots == EArrangeShots::OnTwoRowsShifted; } )
    );
}

//--- FGCObject interface

void 
FBoardSequenceEditorToolkit::AddReferencedObjects( FReferenceCollector& iCollector )
{
    iCollector.AddReferencedObject( mBoardSequence );
}

//--- FAssetEditorToolkit interface

bool FBoardSequenceEditorToolkit::OnRequestClose()
{
    return true;
}

bool FBoardSequenceEditorToolkit::CanFindInContentBrowser() const
{
    // False so that sequencer doesn't take over Find In Content Browser functionality and always find the level sequence asset.
    return false;
}

//--- IToolkit interface

FText FBoardSequenceEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Board Sequence Editor" );
}

FName FBoardSequenceEditorToolkit::GetToolkitFName() const
{
    static FName sSequencerName( "BoardSequenceEditor" );
    return sSequencerName;
}

FString FBoardSequenceEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Sequencer " ).ToString();
}

FLinearColor FBoardSequenceEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.7, 0.0f, 0.2f, 0.5f );
}

void FBoardSequenceEditorToolkit::RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    if( IsWorldCentricAssetEditor() )
    {
        return;
    }

    checkf( false, TEXT( "should never go here as it should always be world-centric" ) );
}

void FBoardSequenceEditorToolkit::UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    if( !IsWorldCentricAssetEditor() )
    {
        checkf( false, TEXT( "should never go here as it should always be world-centric" ) );
    }

    FLevelEditorModule& levelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    levelEditorModule.AttachSequencer( SNullWidget::NullWidget, nullptr );
}

//---

//TSharedRef<FExtender> FTemplateSequenceEditorToolkit::HandleMenuExtensibilityGetExtender( const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects )
//{
//}
//
//void FTemplateSequenceEditorToolkit::HandleTrackMenuExtensionAddTrack( FMenuBuilder& AddTrackMenuBuilder, TArray<UObject*> ContextObjects )
//{
//}
//
//void FTemplateSequenceEditorToolkit::HandleAddComponentActionExecute( UActorComponent* Component )
//{
//}

void
FBoardSequenceEditorToolkit::HandleArrangeShots( EArrangeShots iArrangeShots )
{
    UEposEditorSettings* settings = GetMutableDefault<UEposEditorSettings>();
    settings->BoardTrackSettings.ArrangeShots = iArrangeShots;

    HandleArrangeShots();
}

void
FBoardSequenceEditorToolkit::HandleArrangeShots()
{
    auto track = mBoardSequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return;

    const UEposEditorSettings* settings = GetDefault<UEposEditorSettings>();

    auto sections = track->GetAllSections();
    for( int i = 0; i < sections.Num(); i++ )
    {
        auto section = sections[i];

        section->Modify();

        switch( settings->BoardTrackSettings.ArrangeShots )
        {
            case EArrangeShots::OnOneRow:
                section->SetRowIndex( 0 );
                break;
            default:
            case EArrangeShots::OnTwoRowsShifted:
                section->SetRowIndex( i % 2 );
                break;
        }
    }

    mSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

void FBoardSequenceEditorToolkit::HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding )
{
}

void FBoardSequenceEditorToolkit::HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType )
{
    if( ( iMapChangeType == EMapChangeType::LoadMap || iMapChangeType == EMapChangeType::NewMap || iMapChangeType == EMapChangeType::TearDownWorld ) )
    {
        mSequencer->GetSpawnRegister().CleanUp( *mSequencer );
        CloseWindow();
    }
}

void FBoardSequenceEditorToolkit::OnSequencerReceivedFocus()
{
    if( mSequencer.IsValid() )
    {
        FLevelEditorSequencerIntegration::Get().OnSequencerReceivedFocus( mSequencer.ToSharedRef() );
    }
}

#undef LOCTEXT_NAMESPACE
