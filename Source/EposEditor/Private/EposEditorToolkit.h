// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Styling/ISlateStyle.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "Settings/EposEditorSettings.h"

class FToolBarBuilder;
class FEposEditorPlaybackContext;
class ISequencer;
class FAssetDragDropOp;
class FClassDragDropOp;
class FActorDragDropGraphEdOp;
class UMovieSceneSequence;

/**
 * Implements an Editor toolkit for template sequences.
 */
class FEposEditorToolkit
    : public FAssetEditorToolkit
    , public FGCObject
{
public:

    /**
     * Creates and initializes a new instance.
     *
     * @param InStyle The style set to use.
     */
    FEposEditorToolkit( const TSharedRef<ISlateStyle>& iStyle );

    /** Virtual destructor */
    virtual ~FEposEditorToolkit();

public:

    /**
     * Initialize this asset editor.
     *
     * @param Mode Asset editing mode for this editor (standalone or world-centric).
     * @param InitToolkitHost When Mode is WorldCentric, this is the level editor instance to spawn this editor within.
     * @param iSequences The animation to edit.
     * @param TrackEditorDelegates Delegates to call to create auto-key handlers for this sequencer.
     */
    void Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, TArray< UMovieSceneSequence* > iSequences );

    void GoToFocusedSequence( TArray< UMovieSceneSequence* > iSequences );

public:

    //~ FGCObject interface
    virtual void AddReferencedObjects( FReferenceCollector& iCollector ) override;

    //~ FAssetEditorToolkit interface
    virtual bool OnRequestClose() override;
    virtual bool CanFindInContentBrowser() const override;

    //~ IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual void RegisterTabSpawners( const TSharedRef<FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<FTabManager>& iTabManager ) override;

private:

    /** Callback for the menu extensibility manager. */
    TSharedRef<FExtender> HandleMenuExtensibilityGetExtender( const TSharedRef<FUICommandList> CommandList, const TArray<UObject*> ContextSensitiveObjects );
    /** Callback for the track menu extender. */
    void HandleTrackMenuExtensionAddTrack( FMenuBuilder& AddTrackMenuBuilder, TArray<UObject*> ContextObjects );
    /** Callback for executing the Add Component action. */
    void HandleAddComponentActionExecute( UActorComponent* Component );

    //---

    void HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding );
    void HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType );

    void OnSequencerReceivedFocus();

private:

    void BindCommands( TSharedPtr<FUICommandList> CommandList );

private:

    /** Board or Shot sequence for our edit operation. */
    UMovieSceneSequence* mSequence;

    /** The sequencer used by this editor. */
    TSharedPtr<ISequencer> mSequencer;

    /** Pointer to the style set to use for toolkits. */
    TSharedRef<ISlateStyle> mStyle;

    /** Handle to the sequencer properties menu extender. */
    FDelegateHandle mSequencerExtenderHandle;

    TSharedPtr<FEposEditorPlaybackContext> mPlaybackContext;

    /** The tab ids for all the tabs used */
    static const FName smSequencerMainTabId;
};
