// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#pragma once

#include "EditorSubsystem.h"

#include "MovieSceneTimeUnit.h"
#include "Containers/SortedMap.h"
#include "UObject/StructOnScope.h"
#include "UniversalObjectLocator.h"
#include "UniversalObjectLocatorResolveParams.h"
#include "Misc/NotifyHook.h"
#include "EposSequenceEditorSubsystem.generated.h"

class FUICommandList;
class ISequencer;
class UMovieSceneTrack;
struct FMovieSceneBindingProxy;
struct FMovieScenePasteBindingsParams;
struct FMovieScenePasteFoldersParams;
struct FMovieScenePasteSectionsParams;
struct FMovieScenePasteTracksParams;
struct FBakingAnimationKeySettings;

DECLARE_LOG_CATEGORY_EXTERN(LogEposSequenceEditor, Log, All);

class ACineCameraActor;
class FExtender;
class FMenuBuilder;
class UMovieSceneCompiledDataManager;
class UMovieSceneFolder;
class UMovieSceneSection;
class UMovieSceneSequence;
class USequencerModuleScriptingLayer;
class IStructureDetailsView;
class USequencerCurveEditorObject;

//...

/**
* UEposSequenceEditorSubsystem
* Subsystem for epos sequence editor related utilities to scripts
*/
UCLASS()
class EPOSSEQUENCEEDITOR_API UEposSequenceEditorSubsystem
    : public UEditorSubsystem
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void OnSequencerCreated(TSharedRef<ISequencer> InSequencer);

    void OnSequencerClosed(TSharedRef<ISequencer> InSequencer);

    //...

    /**
     * Copy folders
     * The copied folders will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteFolders if, for example, pasting copy/pasting multiple
     * folders without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    void CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& FoldersExportedText, FString& ObjectsExportedText, FString& TracksExportedText);

    UE_DEPRECATED(5.5, "CopyFolders now gathers objects and tracks within the folders. Please use CopyFolders that outputs ObjectsExportedText and TracksExportedText")
    void CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& FoldersExportedText);

    /**
     * Paste folders
     * Paste folders from the given TextToImport string (used in conjunction with CopyFolders).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    bool PasteFolders(const FString& TextToImport, FMovieScenePasteFoldersParams PasteFoldersParams, TArray<UMovieSceneFolder*>& OutFolders);

    /**
     * Copy sections
     * The copied sections will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteSections if, for example, pasting copy/pasting multiple
     * sections without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    void CopySections(const TArray<UMovieSceneSection*>& Sections, FString& ExportedText);

    /**
     * Paste sections
     * Paste sections from the given TextToImport string (used in conjunction with CopySections).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    bool PasteSections(const FString& TextToImport, FMovieScenePasteSectionsParams PasteSectionsParams, TArray<UMovieSceneSection*>& OutSections);

    /**
     * Copy tracks
     * The copied tracks will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteTracks if, for example, pasting copy/pasting multiple
     * tracks without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    void CopyTracks(const TArray<UMovieSceneTrack*>& Tracks, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText);

    /**
     * Paste tracks
     * Paste tracks from the given TextToImport string (used in conjunction with CopyTracks).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    bool PasteTracks(const FString& TextToImport, FMovieScenePasteTracksParams PasteTracksParams, TArray<UMovieSceneTrack*>& OutTracks);

    /**
     * Copy bindings
     * The copied bindings will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteBindings if, for example, pasting copy/pasting multiple
     * bindings without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    void CopyBindings(const TArray<FMovieSceneBindingProxy>& Bindings, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText);

    /**
     * Paste bindings
     * Paste bindings from the given TextToImport string (used in conjunction with CopyBindings).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    bool PasteBindings(const FString& TextToImport, FMovieScenePasteBindingsParams PasteBindingsParams, TArray<FMovieSceneBindingProxy>& OutObjectBindings);

    //...

    /** Attempts to automatically fix up broken actor references in the current scene */
    UFUNCTION(BlueprintCallable, Category = "Epos Sequence Editor")
    void FixActorReferences();

    //...

private:

    //...

private:

    //...

    TSharedPtr<ISequencer> GetActiveSequencer();

    //...

public:

    //...

private:

    //...

    FDelegateHandle OnSequencerCreatedHandle;

    /* List of sequencers that have been created */
    TArray<TWeakPtr<ISequencer>> Sequencers;

    //...

    TSharedPtr<FUICommandList> CommandList;

    //TSharedPtr<FExtender> TransformMenuExtender;
    TSharedPtr<FExtender> FixActorReferencesMenuExtender;

    //...

};
