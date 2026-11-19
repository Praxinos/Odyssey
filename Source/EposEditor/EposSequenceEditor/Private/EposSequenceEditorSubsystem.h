// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    void CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& FoldersExportedText, FString& ObjectsExportedText, FString& TracksExportedText);

    ///** Copy folders of the focused sequence to the clipboard and return all three text payloads. Raises when no Sequencer is open or a folder belongs elsewhere. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //FMovieSceneFolderClipboardText CopyFoldersToText( const TArray<UMovieSceneFolder*>& Folders );

    UE_DEPRECATED(5.5, "CopyFolders now gathers objects and tracks within the folders. Please use CopyFolders that outputs ObjectsExportedText and TracksExportedText")
    void CopyFolders(const TArray<UMovieSceneFolder*>& Folders, FString& FoldersExportedText);

    /**
     * Paste folders
     * Paste folders from the given TextToImport string (used in conjunction with CopyFolders).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    bool PasteFolders(const FString& TextToImport, FMovieScenePasteFoldersParams PasteFoldersParams, TArray<UMovieSceneFolder*>& OutFolders);

    /**
     * Copy sections
     * The copied sections will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteSections if, for example, pasting copy/pasting multiple
     * sections without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    void CopySections(const TArray<UMovieSceneSection*>& Sections, FString& ExportedText);

    ///** Copy sections to the clipboard and return the text accepted by PasteSectionsFromText. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //FString CopySectionsToText( const TArray<UMovieSceneSection*>& Sections );

    /**
     * Paste sections
     * Paste sections from the given TextToImport string (used in conjunction with CopySections).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    bool PasteSections(const FString& TextToImport, FMovieScenePasteSectionsParams PasteSectionsParams, TArray<UMovieSceneSection*>& OutSections);

    /**
     * Copy tracks
     * The copied tracks will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteTracks if, for example, pasting copy/pasting multiple
     * tracks without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    void CopyTracks(const TArray<UMovieSceneTrack*>& Tracks, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText);

    ///** Copy tracks to the clipboard and return the text accepted by PasteTracksFromText. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //FString CopyTracksToText( const TArray<UMovieSceneTrack*>& Tracks );

    /**
     * Paste tracks
     * Paste tracks from the given TextToImport string (used in conjunction with CopyTracks).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    bool PasteTracks(const FString& TextToImport, FMovieScenePasteTracksParams PasteTracksParams, TArray<UMovieSceneTrack*>& OutTracks);

    /**
     * Copy bindings
     * The copied bindings will be saved to the clipboard as well as assigned to the ExportedText string.
     * The ExportedTest string can be used in conjunction with PasteBindings if, for example, pasting copy/pasting multiple
     * bindings without relying on a single clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    void CopyBindings(const TArray<FMovieSceneBindingProxy>& Bindings, const TArray<UMovieSceneFolder*>& Folders, FString& ExportedText);

    ///** Copy bindings of the focused sequence to the clipboard and return the text accepted by PasteBindingsFromText. Raises when no Sequencer is open or a binding belongs elsewhere. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //FString CopyBindingsToText( const TArray<FMovieSceneBindingProxy>& Bindings );

    /**
     * Paste bindings
     * Paste bindings from the given TextToImport string (used in conjunction with CopyBindings).
     * If TextToImport is empty, the contents of the clipboard will be used.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    bool PasteBindings(const FString& TextToImport, FMovieScenePasteBindingsParams PasteBindingsParams, TArray<FMovieSceneBindingProxy>& OutObjectBindings);

    ///** Paste bindings and return the new objects, raising a script error if nothing was pasted. Empty text uses the clipboard. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //TArray<FMovieSceneBindingProxy> PasteBindingsFromText( const FString& TextToImport, FMovieScenePasteBindingsParams PasteBindingsParams );

    ///** Paste tracks and return the new objects, raising a script error if nothing was pasted. Empty text uses the clipboard. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //TArray<UMovieSceneTrack*> PasteTracksFromText( const FString& TextToImport, FMovieScenePasteTracksParams PasteTracksParams );

    ///** Paste sections and return the new objects, raising a script error if nothing was pasted. Empty text uses the clipboard. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //TArray<UMovieSceneSection*> PasteSectionsFromText( const FString& TextToImport, FMovieScenePasteSectionsParams PasteSectionsParams );

    ///** Paste folders and return the new objects, raising a script error if nothing was pasted. Empty text uses the clipboard. */
    //UFUNCTION( meta = ( PythonCallable, AICallable, AIAccessMode = "Write" ) )
    //TArray<UMovieSceneFolder*> PasteFoldersFromText( const FString& TextToImport, FMovieScenePasteFoldersParams PasteFoldersParams );

    //...

    /** Attempts to automatically fix up broken actor references in the current scene */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Storyboard Editor")
    void FixActorReferences();

    //...

private:

    //...

private:

    //...

    TSharedPtr<ISequencer> GetActiveSequencer() const;

    //...

public:

    //...

private:

    //...

    FDelegateHandle OnSequencerCreatedHandle;

    /* List of sequencers that have been created */
    TArray<TWeakPtr<ISequencer>> Sequencers;

    //...

};
