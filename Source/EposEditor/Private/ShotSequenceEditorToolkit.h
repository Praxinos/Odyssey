// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Shot/ShotSequence.h"
#include "UObject/GCObject.h"
#include "Styling/ISlateStyle.h"
#include "Toolkits/AssetEditorToolkit.h"

class FToolBarBuilder;
class FShotSequenceEditorPlaybackContext;
class ISequencer;
class FAssetDragDropOp;
class FClassDragDropOp;
class FActorDragDropGraphEdOp;

/**
 * Implements an Editor toolkit for template sequences.
 */
class FShotSequenceEditorToolkit : public FAssetEditorToolkit, public FGCObject
{
public:

    /**
     * Creates and initializes a new instance.
     *
     * @param InStyle The style set to use.
     */
    FShotSequenceEditorToolkit( const TSharedRef<ISlateStyle>& iStyle );

    /** Virtual destructor */
    virtual ~FShotSequenceEditorToolkit();

public:

    /**
     * Initialize this asset editor.
     *
     * @param Mode Asset editing mode for this editor (standalone or world-centric).
     * @param InitToolkitHost When Mode is WorldCentric, this is the level editor instance to spawn this editor within.
     * @param ShotSequence The animation to edit.
     * @param TrackEditorDelegates Delegates to call to create auto-key handlers for this sequencer.
     */
    void Initialize( const EToolkitMode::Type iMode, const TSharedPtr<IToolkitHost>& iInitToolkitHost, UShotSequence* iTemplateSequence );

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
    /** Callback for executing the Add Camera command. */
    void HandleCreateCamera();
    /** Callback for executing the Snap Camera command. */
    void HandleSnapCameraToViewport();

    void HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding );
    void HandleMapChanged( UWorld* iNewWorld, EMapChangeType iMapChangeType );

    void OnSequencerReceivedFocus();

private:

    void BindCommands( TSharedPtr<FUICommandList> CommandList );

private:

    /** Shot sequence for our edit operation. */
    UShotSequence* mShotSequence;

    /** The sequencer used by this editor. */
    TSharedPtr<ISequencer> mSequencer;

    /** Pointer to the style set to use for toolkits. */
    TSharedRef<ISlateStyle> mStyle;

    /** Handle to the sequencer properties menu extender. */
    FDelegateHandle mSequencerExtenderHandle;

    TSharedPtr<FShotSequenceEditorPlaybackContext> mPlaybackContext;

    /**	The tab ids for all the tabs used */
    static const FName smSequencerMainTabId;
};
