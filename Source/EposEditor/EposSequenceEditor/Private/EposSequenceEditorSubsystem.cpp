// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceEditorSubsystem.h"
//#include "MVVM/ViewModels/SequencerEditorViewModel.h"
//#include "Scripting/SequencerScriptingLayer.h"

//#include "Evaluation/MovieScenePlayback.h"
#include "ISequencerModule.h"
#include "EposMovieSceneSequence.h" //#include "LevelSequence.h"
//#include "ISceneOutliner.h"
//#include "MovieScenePossessable.h"
#include "MovieScene.h"
#include "MovieSceneFolder.h"
//#include "MovieSceneSpawnable.h"
#include "SequencerUtilities.h"
//#include "Sections/MovieScene3DConstraintSection.h"
//#include "Selection.h"
//#include "Tracks/MovieScene3DTransformTrack.h"
#include "Toolkits/AssetEditorToolkit.h"
//#include "Tracks/MovieScene3DConstraintTrack.h"
//#include "Tracks/MovieSceneCameraShakeTrack.h"

//#include "Editor.h"
//#include "PropertyEditorModule.h"
//#include "Widgets/SWindow.h"
//#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Modules/ModuleManager.h"

//#include "Camera/CameraComponent.h"
//#include "ClassViewerFilter.h"
//#include "ClassViewerModule.h"
//#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
//#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Modules/ModuleManager.h"
#include "ScopedTransaction.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
//#include "BakingAnimationKeySettings.h"
//#include "IStructureDetailsView.h"
//#include "Widgets/SWidget.h"
//#include "Widgets/SCompoundWidget.h"
#include "Widgets/Layout/SBox.h"
//#include "Widgets/Input/NumericTypeInterface.h"
//#include "FrameNumberDetailsCustomization.h"
//#include "MovieSceneToolHelpers.h"
//#include "ActorForWorldTransforms.h"
//#include "KeyParams.h"
#include "EposSequenceEditorBlueprintLibrary.h"

#include "LevelSequenceEditorSubsystem.h"
#include "MovieSceneBindingReferences.h"
#include "EposSequenceEditorScriptingAccess.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EposSequenceEditorSubsystem)

using UE::EposSequenceEditor::Private::GetScriptableMovieScene;
using UE::EposSequenceEditor::Private::RaiseScriptingError;

DEFINE_LOG_CATEGORY(LogEposSequenceEditor);

#define LOCTEXT_NAMESPACE "EposSequenceEditor"

void UEposSequenceEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOGF( LogEposSequenceEditor, Log, "EposSequenceEditor subsystem initialized." );

    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
    OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateUObject(this, &UEposSequenceEditorSubsystem::OnSequencerCreated));

    //...

}

void UEposSequenceEditorSubsystem::Deinitialize()
{
    UE_LOGF( LogEposSequenceEditor, Log, "EposSequenceEditor subsystem deinitialized." );

    ISequencerModule* SequencerModulePtr = FModuleManager::Get().GetModulePtr<ISequencerModule>("Sequencer");
    if (SequencerModulePtr)
    {
        SequencerModulePtr->UnregisterOnSequencerCreated(OnSequencerCreatedHandle);
    }

    //...
}

void UEposSequenceEditorSubsystem::OnSequencerCreated(TSharedRef<ISequencer> InSequencer)
{
    UE_LOGF( LogEposSequenceEditor, VeryVerbose, "UEposSequenceEditorSubsystem::OnSequencerCreated" );

    Sequencers.Add(TWeakPtr<ISequencer>(InSequencer));
    InSequencer->OnCloseEvent().AddUObject(this, &UEposSequenceEditorSubsystem::OnSequencerClosed);

    //CreatePlaybackDelegates( InSequencer );
}

void UEposSequenceEditorSubsystem::OnSequencerClosed(TSharedRef<ISequencer> InSequencer)
{
    //...
}

TSharedPtr<ISequencer> UEposSequenceEditorSubsystem::GetActiveSequencer() const
{
    // Prefer the Epos Sequence editor's Sequencer, which the subsystem's async, outliner and curve editor functions already address.
    // The Sequencers array is in creation order and also holds Sequencers that other hosts, such as Motion Design, opened on epos
    // sequences, so without this an editor opened earlier would be returned instead of the one in use.
    if( const TSharedPtr<ISequencer> EditorSequencer = UEposSequenceEditorBlueprintLibrary::GetCurrentSequencer() )
    {
        UMovieSceneSequence* const Sequence = EditorSequencer->GetFocusedMovieSceneSequence();
        if( Sequence && Sequence->IsA<UEposMovieSceneSequence>() )
        {
            return EditorSequencer;
        }
    }

    for (TWeakPtr<ISequencer> Ptr : Sequencers)
    {
        if (Ptr.IsValid())
        {
            UMovieSceneSequence* Sequence = Ptr.Pin()->GetFocusedMovieSceneSequence();
            if (Sequence && Sequence->IsA<UEposMovieSceneSequence>())
            {
                return Ptr.Pin();
            }
        }
    }

    return nullptr;
}

//...

namespace
{
    /** Raises every paste refusal, or a generic error when the paste failed without reporting one. */
    void RaisePasteErrors( const TArray<FNotificationInfo>& PasteErrors, const TCHAR* PastedKind )
    {
        if( PasteErrors.IsEmpty() )
        {
            RaiseScriptingError( FString::Printf( TEXT( "No %s were pasted. Check the destination and clipboard text." ), PastedKind ) );
        }
        for( const FNotificationInfo& PasteError : PasteErrors )
        {
            RaiseScriptingError( PasteError.Text.Get().ToString() );
        }
    }

    /** The focused movie scene of the open Sequencer; raises and returns null when there is none. */
    UMovieScene* GetFocusedMovieSceneOrRaise( const TSharedPtr<ISequencer>& Sequencer )
    {
        if( !Sequencer )
        {
            RaiseScriptingError( TEXT( "No epos sequence is open in Sequencer" ) );
            return nullptr;
        }
        const UMovieSceneSequence* FocusedSequence = Sequencer->GetFocusedMovieSceneSequence();
        UMovieScene* MovieScene = FocusedSequence ? FocusedSequence->GetMovieScene() : nullptr;
        if( !MovieScene )
        {
            RaiseScriptingError( TEXT( "Sequencer has no focused movie scene" ) );
        }
        return MovieScene;
    }

    //bool CanEditBinding( const FMovieSceneBindingProxy& Binding, const UMovieSceneSequence* Sequence, UMovieScene* MovieScene )
    //{
    //    if( Binding.Sequence.Get() != Sequence || !Binding.BindingID.IsValid()
    //        || !MovieScene->FindPossessable( Binding.BindingID ) )
    //    {
    //        RaiseScriptingError( TEXT( "The actor binding must be a possessable in the focused epos sequence" ) );
    //        return false;
    //    }
    //    return true;
    //}

    //bool CanEditActors( const TArray<AActor*>& Actors, TSharedRef<ISequencer> Sequencer )
    //{
    //    const UObject* PlaybackContext = Sequencer->GetPlaybackContext();
    //    const UWorld* PlaybackWorld = IsValid( PlaybackContext ) ? PlaybackContext->GetWorld() : nullptr;
    //    if( !IsValid( PlaybackWorld ) )
    //    {
    //        RaiseScriptingError( TEXT( "A valid sequencer playback world is required" ) );
    //        return false;
    //    }
    //    for( const AActor* Actor : Actors )
    //    {
    //        if( !IsValid( Actor ) )
    //        {
    //            RaiseScriptingError( TEXT( "All actors must be valid" ) );
    //            return false;
    //        }
    //        if( Actor->GetWorld() != PlaybackWorld )
    //        {
    //            RaiseScriptingError( TEXT( "All actors must belong to the sequencer playback world" ) );
    //            return false;
    //        }
    //    }
    //    return true;
    //}

    //bool CanRemoveActorsFromBinding( const TArray<AActor*>& Actors, TSharedRef<ISequencer> Sequencer, const FGuid& BindingID )
    //{
    //    if( Actors.IsEmpty() )
    //    {
    //        RaiseScriptingError( TEXT( "At least one actor to remove is required" ) );
    //        return false;
    //    }
    //    const TArrayView<TWeakObjectPtr<>> BoundObjects = Sequencer->FindObjectsInCurrentSequence( BindingID );
    //    for( const AActor* Actor : Actors )
    //    {
    //        const bool bIsBound = BoundObjects.ContainsByPredicate( [Actor]( const TWeakObjectPtr<>& BoundObject )
    //                                                                {
    //                                                                    return BoundObject.Get() == Actor;
    //                                                                } );
    //        if( !bIsBound )
    //        {
    //            RaiseScriptingError( TEXT( "All actors to remove must be bound to the target binding" ) );
    //            return false;
    //        }
    //    }
    //    return true;
    //}
}

//...

namespace
{
    bool ContainsFolder( UMovieScene* MovieScene, const UMovieSceneFolder* Target )
    {
        if( !IsValid( MovieScene ) || !IsValid( Target ) || Target->GetTypedOuter<UMovieScene>() != MovieScene )
        {
            return false;
        }
        TArray<const UMovieSceneFolder*> PendingFolders;
        for( const UMovieSceneFolder* RootFolder : MovieScene->GetRootFolders() )
        {
            PendingFolders.Add( RootFolder );
        }
        TSet<const UMovieSceneFolder*> VisitedFolders;
        while( !PendingFolders.IsEmpty() )
        {
            const UMovieSceneFolder* Folder = PendingFolders.Pop( EAllowShrinking::No );
            if( !IsValid( Folder ) || VisitedFolders.Contains( Folder ) )
            {
                continue;
            }
            if( Folder == Target )
            {
                return true;
            }
            VisitedFolders.Add( Folder );
            for( const UMovieSceneFolder* ChildFolder : Folder->GetChildFolders() )
            {
                PendingFolders.Add( ChildFolder );
            }
        }
        return false;
    }

    bool CanAccessClipboardObject( UObject* Object, bool bRequireWritable, UMovieScene*& OutMovieScene )
    {
        // A movie scene or sequence can itself be the clipboard object, for example a movie scene used as a
        // decoration container, so the object counts as its own owner.
        UMovieSceneSequence* Sequence = Cast<UMovieSceneSequence>( Object );
        if( !Sequence && IsValid( Object ) )
        {
            Sequence = Object->GetTypedOuter<UMovieSceneSequence>();
        }
        OutMovieScene = GetScriptableMovieScene( Sequence, bRequireWritable, TEXT( "epos sequence" ) );
        if( !OutMovieScene || !Sequence )
        {
            return false;
        }
        const UMovieScene* ObjectMovieScene = Cast<UMovieScene>( Object );
        if( !ObjectMovieScene )
        {
            ObjectMovieScene = Object == Sequence ? Sequence->GetMovieScene() : Object->GetTypedOuter<UMovieScene>();
        }
        if( ObjectMovieScene != OutMovieScene )
        {
            RaiseScriptingError( TEXT( "The clipboard object must belong to its epos sequence" ) );
            return false;
        }
        return true;
    }

    bool CanAccessClipboardTrack( UMovieSceneTrack* Track, bool bRequireWritable, UMovieScene*& OutMovieScene )
    {
        if( !CanAccessClipboardObject( Track, bRequireWritable, OutMovieScene ) )
        {
            return false;
        }
        FGuid BindingGuid;
        if( !OutMovieScene->ContainsTrack( *Track )
            && OutMovieScene->GetCameraCutTrack() != Track
            && !OutMovieScene->FindTrackBinding( *Track, BindingGuid ) )
        {
            RaiseScriptingError( TEXT( "The track must belong to its epos sequence" ) );
            return false;
        }
        return true;
    }

    bool CanPasteFoldersInto( const FMovieScenePasteFoldersParams& PasteFoldersParams )
    {
        if( !PasteFoldersParams.Sequence )
        {
            RaiseScriptingError( TEXT( "A destination sequence is required" ) );
            return false;
        }
        UMovieScene* MovieScene = GetScriptableMovieScene( PasteFoldersParams.Sequence.Get(), true, TEXT( "epos sequence" ) );
        if( !MovieScene )
        {
            return false;
        }
        if( PasteFoldersParams.ParentFolder && !ContainsFolder( MovieScene, PasteFoldersParams.ParentFolder ) )
        {
            RaiseScriptingError( TEXT( "The parent folder must belong to the destination epos sequence" ) );
            return false;
        }
        return true;
    }

    bool CanPasteSectionsInto( const FMovieScenePasteSectionsParams& PasteSectionsParams )
    {
        bool bHasDestination = PasteSectionsParams.Tracks.ContainsByPredicate( []( const UMovieSceneTrack* Track )
                                                                               {
                                                                                   return Track != nullptr;
                                                                               } );
        for( const FMovieScenePasteDecorationTarget& Target : PasteSectionsParams.DecorationTargets )
        {
            if( !Target.Container && !Target.PreferredDecoration )
            {
                RaiseScriptingError( TEXT( "A decoration target requires a container or a decoration" ) );
                return false;
            }
            bHasDestination = true;
        }
        if( !bHasDestination )
        {
            RaiseScriptingError( TEXT( "A destination track or decoration is required" ) );
            return false;
        }

        for( UMovieSceneTrack* Track : PasteSectionsParams.Tracks )
        {
            UMovieScene* MovieScene = nullptr;
            if( Track && !CanAccessClipboardTrack( Track, true, MovieScene ) )
            {
                return false;
            }
        }
        for( const FMovieScenePasteDecorationTarget& Target : PasteSectionsParams.DecorationTargets )
        {
            UMovieScene* ContainerMovieScene = nullptr;
            UMovieScene* DecorationMovieScene = nullptr;
            if( Target.Container && !CanAccessClipboardObject( Target.Container, true, ContainerMovieScene ) )
            {
                return false;
            }
            if( Target.PreferredDecoration && !CanAccessClipboardObject( Target.PreferredDecoration, true, DecorationMovieScene ) )
            {
                return false;
            }
            if( ContainerMovieScene && DecorationMovieScene && ContainerMovieScene != DecorationMovieScene )
            {
                RaiseScriptingError( TEXT( "The decoration target must belong to its epos sequence" ) );
                return false;
            }
        }
        return true;
    }

    bool CanPasteTracksInto( const FMovieScenePasteTracksParams& PasteTracksParams )
    {
        for( const FMovieSceneBindingProxy& Binding : PasteTracksParams.Bindings )
        {
            const UMovieSceneSequence* BindingSequence = Binding.Sequence.Get();
            const UMovieScene* BindingMovieScene = BindingSequence ? BindingSequence->GetMovieScene() : nullptr;
            if( !BindingMovieScene || !BindingMovieScene->FindBinding( Binding.BindingID ) )
            {
                RaiseScriptingError( TEXT( "The destination binding must belong to its epos sequence" ) );
                return false;
            }
        }
        if( !PasteTracksParams.Sequence && PasteTracksParams.Bindings.IsEmpty() )
        {
            RaiseScriptingError( TEXT( "A destination sequence or binding is required" ) );
            return false;
        }

        UMovieScene* DestinationMovieScene = nullptr;
        if( PasteTracksParams.Sequence )
        {
            DestinationMovieScene = GetScriptableMovieScene( PasteTracksParams.Sequence, true, TEXT( "epos sequence" ) );
            if( !DestinationMovieScene )
            {
                return false;
            }
        }
        for( const FMovieSceneBindingProxy& Binding : PasteTracksParams.Bindings )
        {
            if( !GetScriptableMovieScene( Binding.Sequence.Get(), true, TEXT( "epos sequence" ) ) )
            {
                return false;
            }
        }
        if( PasteTracksParams.ParentFolder && !ContainsFolder( DestinationMovieScene, PasteTracksParams.ParentFolder ) )
        {
            RaiseScriptingError( TEXT( "The parent folder must belong to the destination epos sequence" ) );
            return false;
        }
        for( UMovieSceneFolder* Folder : PasteTracksParams.Folders )
        {
            if( !ContainsFolder( DestinationMovieScene, Folder ) )
            {
                RaiseScriptingError( TEXT( "All destination folders must belong to the destination epos sequence" ) );
                return false;
            }
        }
        return true;
    }

    bool CanPasteBindingsInto( UMovieSceneSequence* FocusedSequence, const FMovieScenePasteBindingsParams& PasteBindingsParams )
    {
        UMovieScene* MovieScene = GetScriptableMovieScene( FocusedSequence, true, TEXT( "epos sequence" ) );
        if( !MovieScene )
        {
            return false;
        }
        for( const FMovieSceneBindingProxy& Binding : PasteBindingsParams.Bindings )
        {
            if( Binding.Sequence.Get() != FocusedSequence || !MovieScene->FindBinding( Binding.BindingID ) )
            {
                RaiseScriptingError( TEXT( "All destination bindings must belong to the focused epos sequence" ) );
                return false;
            }
        }
        if( PasteBindingsParams.ParentFolder && !ContainsFolder( MovieScene, PasteBindingsParams.ParentFolder ) )
        {
            RaiseScriptingError( TEXT( "The parent folder must belong to the focused epos sequence" ) );
            return false;
        }
        for( UMovieSceneFolder* Folder : PasteBindingsParams.Folders )
        {
            if( !ContainsFolder( MovieScene, Folder ) )
            {
                RaiseScriptingError( TEXT( "All destination folders must belong to the focused epos sequence" ) );
                return false;
            }
        }
        return true;
    }
}

void UEposSequenceEditorSubsystem::CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    FString DummyText;
    CopyFolders( Folders, ExportedText, DummyText, DummyText );
}

void UEposSequenceEditorSubsystem::CopyFolders( const TArray<UMovieSceneFolder*>&Folders, FString & FoldersExportedText, FString & ObjectsExportedText, FString & TracksExportedText )
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    UMovieScene* MovieScene = GetFocusedMovieSceneOrRaise( Sequencer );
    if( !MovieScene )
    {
        return;
    }
    for( const UMovieSceneFolder* Folder : Folders )
    {
        if( !ContainsFolder( MovieScene, Folder ) )
        {
            RaiseScriptingError( TEXT( "All folders must belong to the focused epos sequence" ) );
            return;
        }
    }
    if( !GetScriptableMovieScene( Sequencer->GetFocusedMovieSceneSequence(), false, TEXT( "epos sequence" ) ) )
    {
        return;
    }

    FSequencerUtilities::CopyFolders( Sequencer.ToSharedRef(), Folders, FoldersExportedText, TracksExportedText, ObjectsExportedText );

    FString ExportedText;
    ExportedText += ObjectsExportedText;
    ExportedText += TracksExportedText;
    ExportedText += FoldersExportedText;

    FPlatformApplicationMisc::ClipboardCopy( *ExportedText );
}

bool UEposSequenceEditorSubsystem::PasteFolders(const FString& InTextToImport, FMovieScenePasteFoldersParams PasteFoldersParams, TArray<UMovieSceneFolder*>& OutFolders)
{
    if( !CanPasteFoldersInto( PasteFoldersParams ) )
    {
        return false;
    }

    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteFolders(TextToImport, PasteFoldersParams, OutFolders, PasteErrors))
    {
        RaisePasteErrors( PasteErrors, TEXT( "folders" ) );
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopySections(const TArray<UMovieSceneSection*>& Sections, FString& ExportedText)
{
    for( UMovieSceneSection* Section : Sections )
    {
        UMovieScene* MovieScene = nullptr;
        if( !CanAccessClipboardObject( Section, false, MovieScene ) )
        {
            return;
        }
    }

    FSequencerUtilities::CopySections(Sections, ExportedText);

    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool UEposSequenceEditorSubsystem::PasteSections(const FString& InTextToImport, FMovieScenePasteSectionsParams PasteSectionsParams, TArray<UMovieSceneSection*>& OutSections)
{
    if( !CanPasteSectionsInto( PasteSectionsParams ) )
    {
        return false;
    }

    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteSections(TextToImport, PasteSectionsParams, OutSections, PasteErrors))
    {
        RaisePasteErrors( PasteErrors, TEXT( "sections" ) );
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopyTracks(const TArray<UMovieSceneTrack*>& Tracks, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    for( UMovieSceneTrack* Track : Tracks )
    {
        UMovieScene* MovieScene = nullptr;
        if( !CanAccessClipboardTrack( Track, false, MovieScene ) )
        {
            return;
        }
    }

    //TArray<UMovieSceneFolder*> Folders;
    FSequencerUtilities::CopyTracks(Tracks, Folders, ExportedText);
    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool UEposSequenceEditorSubsystem::PasteTracks(const FString& InTextToImport, FMovieScenePasteTracksParams PasteTracksParams, TArray<UMovieSceneTrack*>& OutTracks)
{
    if( !CanPasteTracksInto( PasteTracksParams ) )
    {
        return false;
    }

    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteTracks(TextToImport, PasteTracksParams, OutTracks, PasteErrors))
    {
        RaisePasteErrors( PasteErrors, TEXT( "tracks" ) );
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopyBindings(const TArray<FMovieSceneBindingProxy>& Bindings, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    const UMovieScene* MovieScene = GetFocusedMovieSceneOrRaise( Sequencer );
    if( !MovieScene )
    {
        return;
    }
    for( const FMovieSceneBindingProxy& Binding : Bindings )
    {
        if( Binding.Sequence.Get() != Sequencer->GetFocusedMovieSceneSequence() || !MovieScene->FindBinding( Binding.BindingID ) )
        {
            RaiseScriptingError( TEXT( "All copied bindings must belong to the focused epos sequence" ) );
            return;
        }
    }
    if( !GetScriptableMovieScene( Sequencer->GetFocusedMovieSceneSequence(), false, TEXT( "epos sequence" ) ) )
    {
        return;
    }

    //TArray<UMovieSceneFolder*> Folders;
    FSequencerUtilities::CopyBindings( Sequencer.ToSharedRef(), Bindings, Folders, ExportedText );
    FPlatformApplicationMisc::ClipboardCopy( *ExportedText );
}

bool UEposSequenceEditorSubsystem::PasteBindings(const FString& InTextToImport, FMovieScenePasteBindingsParams PasteBindingsParams, TArray<FMovieSceneBindingProxy>& OutObjectBindings)
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    const UMovieScene* MovieScene = GetFocusedMovieSceneOrRaise( Sequencer );
    if( !MovieScene )
    {
        return false;
    }
    if( !CanPasteBindingsInto( Sequencer->GetFocusedMovieSceneSequence(), PasteBindingsParams ) )
    {
        return false;
    }

    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteBindings(TextToImport, Sequencer.ToSharedRef(), PasteBindingsParams, OutObjectBindings, PasteErrors))
    {
        RaisePasteErrors( PasteErrors, TEXT( "bindings" ) );
        return false;
    }

    return true;
}

//FString UEposSequenceEditorSubsystem::CopyBindingsToText( const TArray<FMovieSceneBindingProxy>& Bindings )
//{
//    FString ExportedText;
//    CopyBindings( Bindings, ExportedText );
//    return ExportedText;
//}
//
//FString UEposSequenceEditorSubsystem::CopyTracksToText( const TArray<UMovieSceneTrack*>& Tracks )
//{
//    FString ExportedText;
//    CopyTracks( Tracks, ExportedText );
//    return ExportedText;
//}
//
//FString UEposSequenceEditorSubsystem::CopySectionsToText( const TArray<UMovieSceneSection*>& Sections )
//{
//    FString ExportedText;
//    CopySections( Sections, ExportedText );
//    return ExportedText;
//}
//
//FMovieSceneFolderClipboardText UEposSequenceEditorSubsystem::CopyFoldersToText( const TArray<UMovieSceneFolder*>& Folders )
//{
//    FMovieSceneFolderClipboardText Result;
//    CopyFolders( Folders, Result.FoldersExportedText, Result.ObjectsExportedText, Result.TracksExportedText );
//    return Result;
//}
//
//TArray<FMovieSceneBindingProxy> UEposSequenceEditorSubsystem::PasteBindingsFromText( const FString& TextToImport, FMovieScenePasteBindingsParams PasteBindingsParams )
//{
//    TArray<FMovieSceneBindingProxy> Result;
//    if( !PasteBindings( TextToImport, PasteBindingsParams, Result ) )
//    {
//        return {};
//    }
//    return Result;
//}
//
//TArray<UMovieSceneTrack*> UEposSequenceEditorSubsystem::PasteTracksFromText( const FString& TextToImport, FMovieScenePasteTracksParams PasteTracksParams )
//{
//    TArray<UMovieSceneTrack*> Result;
//    if( !PasteTracks( TextToImport, PasteTracksParams, Result ) )
//    {
//        return {};
//    }
//    return Result;
//}
//
//TArray<UMovieSceneSection*> UEposSequenceEditorSubsystem::PasteSectionsFromText( const FString& TextToImport, FMovieScenePasteSectionsParams PasteSectionsParams )
//{
//    TArray<UMovieSceneSection*> Result;
//    if( !PasteSections( TextToImport, PasteSectionsParams, Result ) )
//    {
//        return {};
//    }
//    return Result;
//}
//
//TArray<UMovieSceneFolder*> UEposSequenceEditorSubsystem::PasteFoldersFromText( const FString& TextToImport, FMovieScenePasteFoldersParams PasteFoldersParams )
//{
//    TArray<UMovieSceneFolder*> Result;
//    if( !PasteFolders( TextToImport, PasteFoldersParams, Result ) )
//    {
//        return {};
//    }
//    return Result;
//}

//...

namespace
{
    /**
     * The focused movie scene of an open Sequencer when scripting may edit its bindings. Raises and returns null
     * when no sequence is open or the focused sequence is hidden or read-only.
     */
    UMovieScene* GetEditableFocusedMovieScene( const TSharedPtr<ISequencer>& Sequencer )
    {
        if( !GetFocusedMovieSceneOrRaise( Sequencer ) )
        {
            return nullptr;
        }
        return GetScriptableMovieScene( Sequencer->GetFocusedMovieSceneSequence(), true, TEXT( "focused epos sequence" ) );
    }

    ///**
    // * Whether a binding edit may proceed: the focused sequence may be edited and the binding is one of its
    // * possessables. FSequencerUtilities edits the movie scene of the binding's own sequence, so a binding of any
    // * other sequence is refused. Actor edits also need a binding that holds assigned actors: spawnable and other
    // * custom bindings are stored as possessables too, but they resolve their objects themselves.
    // */
    //bool CanEditFocusedBinding( const TSharedPtr<ISequencer>& Sequencer, const FMovieSceneBindingProxy& Binding, bool bIsActorEdit )
    //{
    //    UMovieScene* MovieScene = GetEditableFocusedMovieScene( Sequencer );
    //    if( !MovieScene || !CanEditBinding( Binding, Sequencer->GetFocusedMovieSceneSequence(), MovieScene ) )
    //    {
    //        return false;
    //    }
    //    const FMovieSceneBindingReferences* BindingReferences = Sequencer->GetFocusedMovieSceneSequence()->GetBindingReferences();
    //    if( bIsActorEdit && BindingReferences && BindingReferences->GetCustomBinding( Binding.BindingID, 0 ) )
    //    {
    //        RaiseScriptingError( TEXT( "Actors can only be added to or removed from a possessable binding" ) );
    //        return false;
    //    }
    //    return true;
    //}
}

void UEposSequenceEditorSubsystem::FixActorReferences()
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    if( GetEditableFocusedMovieScene( Sequencer ) )
    {
        FSequencerUtilities::FixActorReferences( Sequencer.ToSharedRef() );
    }
}

//...

#undef LOCTEXT_NAMESPACE
