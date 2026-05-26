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
//#include "MovieScene.h"
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

#include "LevelSequenceEditorSubsystem.h"
#include "MovieSceneBindingReferences.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EposSequenceEditorSubsystem)

DEFINE_LOG_CATEGORY(LogEposSequenceEditor);

#define LOCTEXT_NAMESPACE "EposSequenceEditor"

void UEposSequenceEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOGF( LogEposSequenceEditor, Log, "LevelSequenceEditor subsystem initialized." );

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
}

void UEposSequenceEditorSubsystem::OnSequencerClosed(TSharedRef<ISequencer> InSequencer)
{
    //...
}

TSharedPtr<ISequencer> UEposSequenceEditorSubsystem::GetActiveSequencer()
{
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

void UEposSequenceEditorSubsystem::CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    FString DummyText;
    CopyFolders( Folders, ExportedText, DummyText, DummyText );
}

void UEposSequenceEditorSubsystem::CopyFolders( const TArray<UMovieSceneFolder*>&Folders, FString & FoldersExportedText, FString & ObjectsExportedText, FString & TracksExportedText )
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    if( Sequencer == nullptr )
    {
        return;
    }

    FSequencerUtilities::CopyFolders( Sequencer.ToSharedRef(), Folders, FoldersExportedText, ObjectsExportedText, TracksExportedText );

    FString ExportedText;
    ExportedText += ObjectsExportedText;
    ExportedText += TracksExportedText;
    ExportedText += FoldersExportedText;

    FPlatformApplicationMisc::ClipboardCopy( *ExportedText );
}

bool UEposSequenceEditorSubsystem::PasteFolders(const FString& InTextToImport, FMovieScenePasteFoldersParams PasteFoldersParams, TArray<UMovieSceneFolder*>& OutFolders)
{
    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteFolders(TextToImport, PasteFoldersParams, OutFolders, PasteErrors))
    {
        for (FNotificationInfo PasteError : PasteErrors)
        {
            UE_LOGF( LogEposSequenceEditor, Error, "%ls", *PasteError.Text.Get().ToString() );
        }
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopySections(const TArray<UMovieSceneSection*>& Sections, FString& ExportedText)
{
    FSequencerUtilities::CopySections(Sections, ExportedText);

    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool UEposSequenceEditorSubsystem::PasteSections(const FString& InTextToImport, FMovieScenePasteSectionsParams PasteSectionsParams, TArray<UMovieSceneSection*>& OutSections)
{
    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteSections(TextToImport, PasteSectionsParams, OutSections, PasteErrors))
    {
        for (FNotificationInfo PasteError : PasteErrors)
        {
            UE_LOGF( LogEposSequenceEditor, Error, "%ls", *PasteError.Text.Get().ToString() );
        }
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopyTracks(const TArray<UMovieSceneTrack*>& Tracks, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    //TArray<UMovieSceneFolder*> Folders;
    FSequencerUtilities::CopyTracks(Tracks, Folders, ExportedText);
    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool UEposSequenceEditorSubsystem::PasteTracks(const FString& InTextToImport, FMovieScenePasteTracksParams PasteTracksParams, TArray<UMovieSceneTrack*>& OutTracks)
{
    FString TextToImport = InTextToImport;
    if (TextToImport.IsEmpty())
    {
        FPlatformApplicationMisc::ClipboardPaste(TextToImport);
    }

    TArray<FNotificationInfo> PasteErrors;
    if (!FSequencerUtilities::PasteTracks(TextToImport, PasteTracksParams, OutTracks, PasteErrors))
    {
        for (FNotificationInfo PasteError : PasteErrors)
        {
            UE_LOGF( LogEposSequenceEditor, Error, "%ls", *PasteError.Text.Get().ToString() );
        }
        return false;
    }

    return true;
}

void UEposSequenceEditorSubsystem::CopyBindings(const TArray<FMovieSceneBindingProxy>& Bindings, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText)
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    if (Sequencer == nullptr)
    {
        return;
    }

    //TArray<UMovieSceneFolder*> Folders;
    FSequencerUtilities::CopyBindings(Sequencer.ToSharedRef(), Bindings, Folders, ExportedText);
    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool UEposSequenceEditorSubsystem::PasteBindings(const FString& InTextToImport, FMovieScenePasteBindingsParams PasteBindingsParams, TArray<FMovieSceneBindingProxy>& OutObjectBindings)
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    if (Sequencer == nullptr)
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
        for (FNotificationInfo PasteError : PasteErrors)
        {
            UE_LOGF( LogEposSequenceEditor, Error, "%ls", *PasteError.Text.Get().ToString() );
        }
        return false;
    }

    return true;
}

//...

void UEposSequenceEditorSubsystem::FixActorReferences()
{
    if( TSharedPtr<ISequencer> Sequencer = GetActiveSequencer() )
    {
        FSequencerUtilities::FixActorReferences( Sequencer.ToSharedRef() );
    }
}

//...

#undef LOCTEXT_NAMESPACE
