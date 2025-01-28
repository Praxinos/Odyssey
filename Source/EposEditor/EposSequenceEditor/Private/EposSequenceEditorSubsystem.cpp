// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#include "EposSequenceEditorSubsystem.h"
//#include "MVVM/ViewModels/SequencerEditorViewModel.h"
//#include "Scripting/SequencerScriptingLayer.h"

//#include "Evaluation/MovieScenePlayback.h"
#include "ISequencerModule.h"
#include "Framework/Commands/UICommandList.h"
#include "EposMovieSceneSequence.h" //#include "LevelSequence.h"
//#include "ISceneOutliner.h"
#include "EposSequenceEditorCommands.h" //#include "LevelSequenceEditorCommands.h"
//#include "MovieScenePossessable.h"
//#include "SequencerSettings.h"
//#include "MovieScene.h"
//#include "MovieSceneSpawnable.h"
#include "SequencerUtilities.h"
//#include "Sections/MovieScene3DConstraintSection.h"
//#include "Selection.h"
//#include "Tracks/MovieScene3DTransformTrack.h"
#include "Toolkits/AssetEditorToolkit.h"
//#include "Tracks/MovieScene3DConstraintTrack.h"
//#include "Tracks/MovieSceneCameraShakeTrack.h"

//#include "ActorTreeItem.h"
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
//#include "SceneOutlinerModule.h"
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

#include UE_INLINE_GENERATED_CPP_BY_NAME(EposSequenceEditorSubsystem)

DEFINE_LOG_CATEGORY(LogEposSequenceEditor);

#define LOCTEXT_NAMESPACE "EposSequenceEditor"

void UEposSequenceEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    UE_LOG(LogEposSequenceEditor, Log, TEXT("EposSequenceEditor subsystem initialized."));

    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
    OnSequencerCreatedHandle = SequencerModule.RegisterOnSequencerCreated(FOnSequencerCreated::FDelegate::CreateUObject(this, &UEposSequenceEditorSubsystem::OnSequencerCreated));

    //...

    /* Commands for this subsystem */
    CommandList = MakeShareable(new FUICommandList);

    //...

    CommandList->MapAction(FEposSequenceEditorCommands::Get().FixActorReferences,
        FExecuteAction::CreateUObject(this, &UEposSequenceEditorSubsystem::FixActorReferences)
    );

    //...

    FixActorReferencesMenuExtender = MakeShareable(new FExtender);
    FixActorReferencesMenuExtender->AddMenuExtension("Bindings", EExtensionHook::First, CommandList, FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder) {
        // Only add menu entries where the focused sequence is a UEposMovieSceneSequence
        if (!GetActiveSequencer())
        {
            return;
        }

        MenuBuilder.AddMenuEntry(FEposSequenceEditorCommands::Get().FixActorReferences);
        }));

    SequencerModule.GetActionsMenuExtensibilityManager()->AddExtender(FixActorReferencesMenuExtender);

    //...
}

void UEposSequenceEditorSubsystem::Deinitialize()
{
    UE_LOG(LogEposSequenceEditor, Log, TEXT("EposSequenceEditor subsystem deinitialized."));

    ISequencerModule* SequencerModulePtr = FModuleManager::Get().GetModulePtr<ISequencerModule>("Sequencer");
    if (SequencerModulePtr)
    {
        SequencerModulePtr->UnregisterOnSequencerCreated(OnSequencerCreatedHandle);
    }

    //...
}

void UEposSequenceEditorSubsystem::OnSequencerCreated(TSharedRef<ISequencer> InSequencer)
{
    UE_LOG(LogEposSequenceEditor, VeryVerbose, TEXT("UEposSequenceEditorSubsystem::OnSequencerCreated"));

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
            UE_LOG(LogEposSequenceEditor, Error, TEXT("%s"), *PasteError.Text.Get().ToString());
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
            UE_LOG(LogEposSequenceEditor, Error, TEXT("%s"), *PasteError.Text.Get().ToString());
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
            UE_LOG(LogEposSequenceEditor, Error, TEXT("%s"), *PasteError.Text.Get().ToString());
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
            UE_LOG(LogEposSequenceEditor, Error, TEXT("%s"), *PasteError.Text.Get().ToString());
        }
        return false;
    }

    return true;
}

//...

void UEposSequenceEditorSubsystem::FixActorReferences()
{
    TSharedPtr<ISequencer> Sequencer = GetActiveSequencer();
    if (Sequencer == nullptr)
    {
        return;
    }

    UWorld* PlaybackContext = Sequencer->GetPlaybackContext()->GetWorld();
    if (!PlaybackContext)
    {
        return;
    }

    UMovieScene* FocusedMovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
    if (!FocusedMovieScene)
    {
        return;
    }

    if (FocusedMovieScene->IsReadOnly())
    {
        FSequencerUtilities::ShowReadOnlyError();
        return;
    }

    FScopedTransaction FixActorReferencesTransaction(LOCTEXT("FixActorReferences", "Fix Actor References"));

    TMap<FString, AActor*> ActorNameToActorMap;

    for (TActorIterator<AActor> ActorItr(PlaybackContext); ActorItr; ++ActorItr)
    {
        // Same as with the Object Iterator, access the subclass instance with the * or -> operators.
        AActor* Actor = *ActorItr;
        ActorNameToActorMap.Add(Actor->GetActorLabel(), Actor);
    }

    // Cache the possessables to fix up first since the bindings will change as the fix ups happen.
    TArray<FMovieScenePossessable> ActorsPossessablesToFix;
    for (int32 i = 0; i < FocusedMovieScene->GetPossessableCount(); i++)
    {
        FMovieScenePossessable& Possessable = FocusedMovieScene->GetPossessable(i);
        // Possessables with parents are components so ignore them.
        if (Possessable.GetParent().IsValid() == false)
        {
            if (Sequencer->FindBoundObjects(Possessable.GetGuid(), Sequencer->GetFocusedTemplateID()).Num() == 0)
            {
                ActorsPossessablesToFix.Add(Possessable);
            }
        }
    }

    // For the possessables to fix, look up the actors by name and reassign them if found.
    TMap<FGuid, FGuid> OldGuidToNewGuidMap;
    for (const FMovieScenePossessable& ActorPossessableToFix : ActorsPossessablesToFix)
    {
        AActor* ActorPtr = ActorNameToActorMap.FindRef(ActorPossessableToFix.GetName());
        if (ActorPtr != nullptr)
        {
            FGuid OldGuid = ActorPossessableToFix.GetGuid();

            // The actor might have an existing guid while the possessable with the same name might not.
            // In that case, make sure we also replace the existing guid with the new guid
            FGuid ExistingGuid = Sequencer->FindObjectId(*ActorPtr, Sequencer->GetFocusedTemplateID());

            FGuid NewGuid = FSequencerUtilities::AssignActor(Sequencer.ToSharedRef(), ActorPtr, ActorPossessableToFix.GetGuid());

            OldGuidToNewGuidMap.Add(OldGuid, NewGuid);

            if (ExistingGuid.IsValid())
            {
                OldGuidToNewGuidMap.Add(ExistingGuid, NewGuid);
            }
        }
    }

    for (TPair<FGuid, FGuid> GuidPair : OldGuidToNewGuidMap)
    {
        FSequencerUtilities::UpdateBindingIDs(Sequencer.ToSharedRef(), GuidPair.Key, GuidPair.Value);
    }

    Sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

//...

#undef LOCTEXT_NAMESPACE
