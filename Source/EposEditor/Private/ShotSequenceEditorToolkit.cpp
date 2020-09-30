// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "ShotSequenceEditorToolkit.h"

#include "Actor.h"
#include "CineCameraActor.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "Containers\ArrayBuilder.h"
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
#include "Misc/ShotSequenceEditorPlaybackContext.h"
//#include "Misc/TemplateSequenceEditorSpawnRegister.h"
//#include "Misc/TemplateSequenceEditorUtil.h"
#include "Helpers/ShotSequenceHelpers.h"
#include "Modules/ModuleManager.h"
#include "ScopedTransaction.h"
#include "ShotSequenceEditorCommands.h"
#include "SequencerSettings.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "ShotSequenceEditor"

const FName FShotSequenceEditorToolkit::smSequencerMainTabId( TEXT( "Sequencer_SequencerMain" ) );

namespace SequencerDefs
{
    static const FName sgShotSequencerAppIdentifier( TEXT( "ShotSequencerApp" ) );
}

FShotSequenceEditorToolkit::FShotSequenceEditorToolkit( const TSharedRef<ISlateStyle>& iStyle )
    : mShotSequence( nullptr )
    , mStyle( iStyle )
{
    // register sequencer menu extenders
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    int32 NewIndex = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates().Add(
        FAssetEditorExtender::CreateRaw( this, &FShotSequenceEditorToolkit::HandleMenuExtensibilityGetExtender ) );
    mSequencerExtenderHandle = SequencerModule.GetAddTrackMenuExtensibilityManager()->GetExtenderDelegates()[NewIndex].GetHandle();
}

FShotSequenceEditorToolkit::~FShotSequenceEditorToolkit()
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

void FShotSequenceEditorToolkit::Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, UShotSequence* iShotSequence )
{
    // create tab layout
    const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout( "Standalone_ShotSequenceEditor" )
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab( smSequencerMainTabId, ETabState::OpenedTab )
            )
        );

    mShotSequence = iShotSequence;
    mPlaybackContext = MakeShared<FShotSequenceEditorPlaybackContext>();

    // Mode sould always be world-centric (don't know how to have a standalone one)
    // in this case, SequencerDefs::ShotSequencerAppIdentifier & StandaloneDefaultLayout is not useful
    const bool bCreateDefaultStandaloneMenu = true;
    const bool bCreateDefaultToolbar = false;
    FAssetEditorToolkit::InitAssetEditor( iMode, iInitToolkitHost, SequencerDefs::sgShotSequencerAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, mShotSequence );

    //TSharedRef<FTemplateSequenceEditorSpawnRegister> SpawnRegister = MakeShareable( new FTemplateSequenceEditorSpawnRegister() );
    //SpawnRegister->SetSequencer( Sequencer );

    // Initialize sequencer.
    FSequencerInitParams sequencerInitParams;
    {
        sequencerInitParams.RootSequence = mShotSequence;
        sequencerInitParams.bEditWithinLevelEditor = true;
        sequencerInitParams.ToolkitHost = iInitToolkitHost;
        //sequencerInitParams.SpawnRegister = SpawnRegister;
        sequencerInitParams.HostCapabilities.bSupportsCurveEditor = true;
        sequencerInitParams.HostCapabilities.bSupportsSaveMovieSceneAsset = true;

        sequencerInitParams.PlaybackContext.Bind( mPlaybackContext.ToSharedRef(), &FShotSequenceEditorPlaybackContext::GetPlaybackContext );

        sequencerInitParams.ViewParams.UniqueName = "ShotSequenceEditor";
        sequencerInitParams.ViewParams.ScrubberStyle = ESequencerScrubberStyle::FrameBlock;
        sequencerInitParams.ViewParams.OnReceivedFocus.BindRaw( this, &FShotSequenceEditorToolkit::OnSequencerReceivedFocus );
    }

    mSequencer = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" ).CreateSequencer( sequencerInitParams );

    mSequencer->OnActorAddedToSequencer().AddSP( this, &FShotSequenceEditorToolkit::HandleActorAddedToSequencer );

    //if( ToolkitParams.InitialBindingClass != nullptr )
    //{
    //    FTemplateSequenceEditorUtil Util( ShotSequence, *Sequencer.Get() );
    //    Util.ChangeActorBinding( ToolkitParams.InitialBindingClass );
    //}

    // with ToolkitCommands       // -> it doesn't work ( ¯\_O_/¯ ?)
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
    levelEditorModule.OnMapChanged().AddRaw( this, &FShotSequenceEditorToolkit::HandleMapChanged );
}

void
FShotSequenceEditorToolkit::BindCommands( TSharedPtr<FUICommandList> CommandList )
{
    CommandList->MapAction(
        FShotSequenceEditorCommands::Get().CreateCamera,
        FExecuteAction::CreateSP( this, &FShotSequenceEditorToolkit::HandleCreateCamera ),
        FCanExecuteAction::CreateLambda( [this]{ return !ShotSequenceHelpers::GetCamera( mSequencer, nullptr ); } )
    );
    CommandList->MapAction(
        FShotSequenceEditorCommands::Get().SnapCameraToViewport,
        FExecuteAction::CreateSP( this, &FShotSequenceEditorToolkit::HandleSnapCameraToViewport ),
        FCanExecuteAction::CreateLambda( [this]{ return !!ShotSequenceHelpers::GetCamera( mSequencer, nullptr ); } )
    );

}

//--- FGCObject interface

void
FShotSequenceEditorToolkit::AddReferencedObjects( FReferenceCollector& iCollector )
{
    iCollector.AddReferencedObject( mShotSequence );
}

//--- FAssetEditorToolkit interface

bool FShotSequenceEditorToolkit::OnRequestClose()
{
    return true;
}

bool FShotSequenceEditorToolkit::CanFindInContentBrowser() const
{
    // False so that sequencer doesn't take over Find In Content Browser functionality and always find the level sequence asset.
    return false;
}

//--- IToolkit interface

FText FShotSequenceEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Shot Sequence Editor" );
}

FName FShotSequenceEditorToolkit::GetToolkitFName() const
{
    static FName sSequencerName( "ShotSequenceEditor" );
    return sSequencerName;
}

FString FShotSequenceEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Sequencer " ).ToString();
}

FLinearColor FShotSequenceEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.7, 0.0f, 0.2f, 0.5f );
}

void FShotSequenceEditorToolkit::RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    if( IsWorldCentricAssetEditor() )
    {
        return;
    }

    checkf( false, TEXT( "should never go here as it should always be world-centric" ) );
}

void FShotSequenceEditorToolkit::UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
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
FShotSequenceEditorToolkit::HandleMenuExtensibilityGetExtender( const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects )
{
    TSharedRef<FExtender> AddTrackMenuExtender( new FExtender() );
    AddTrackMenuExtender->AddMenuExtension(
        SequencerMenuExtensionPoints::AddTrackMenu_PropertiesSection,
        EExtensionHook::Before,
        CommandList,
        FMenuExtensionDelegate::CreateRaw( this, &FShotSequenceEditorToolkit::HandleTrackMenuExtensionAddTrack, ContextSensitiveObjects ) );

    return AddTrackMenuExtender;
}

void
FShotSequenceEditorToolkit::HandleTrackMenuExtensionAddTrack( FMenuBuilder& AddTrackMenuBuilder, TArray<UObject*> ContextObjects )
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
                FUIAction AddComponentAction( FExecuteAction::CreateSP( this, &FShotSequenceEditorToolkit::HandleAddComponentActionExecute, Component ) );
                FText AddComponentLabel = FText::FromString( Component->GetName() );
                FText AddComponentToolTip = FText::Format( LOCTEXT( "ComponentToolTipFormat", "Add {0} component" ), FText::FromString( Component->GetName() ) );
                AddTrackMenuBuilder.AddMenuEntry( AddComponentLabel, AddComponentToolTip, FSlateIcon(), AddComponentAction );
            }
        }
    }
    AddTrackMenuBuilder.EndSection();
}

void
FShotSequenceEditorToolkit::HandleAddComponentActionExecute( UActorComponent* Component )
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

void FShotSequenceEditorToolkit::HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding )
{
    ShotSequenceHelpers::CreateDefaultInnerTrack( mSequencer, iActor, iBinding );
}

void FShotSequenceEditorToolkit::HandleCreateCamera()
{
    ShotSequenceHelpers::CreateCameraAndCameraCut( mSequencer );
}

void FShotSequenceEditorToolkit::HandleSnapCameraToViewport()
{
    ShotSequenceHelpers::SnapCameraToViewport( mSequencer );
}

void FShotSequenceEditorToolkit::HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType )
{
    if( ( iMapChangeType == EMapChangeType::LoadMap || iMapChangeType == EMapChangeType::NewMap || iMapChangeType == EMapChangeType::TearDownWorld ) )
    {
        mSequencer->GetSpawnRegister().CleanUp( *mSequencer );
        CloseWindow();
    }
}

void FShotSequenceEditorToolkit::OnSequencerReceivedFocus()
{
    if( mSequencer.IsValid() )
    {
        FLevelEditorSequencerIntegration::Get().OnSequencerReceivedFocus( mSequencer.ToSharedRef() );
    }
}

#undef LOCTEXT_NAMESPACE
