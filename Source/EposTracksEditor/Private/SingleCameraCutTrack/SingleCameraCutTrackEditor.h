// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"
#include "MovieSceneToolHelpers.h"

class AActor;
class FMenuBuilder;
class FTrackEditorThumbnailPool;
class UFactory;
class UMovieSceneSingleCameraCutTrack;
class FSingleCameraCutTrackEditor;
class FTrackEditorBindingIDPicker;
struct FMovieSceneObjectBindingID;

/**
 * Tools for camera cut tracks.
 */
class FSingleCameraCutTrackEditor
    : public FMovieSceneTrackEditor
{
public:

    /**
     * Constructor
     *
     * @param InSequencer The sequencer instance to be used by this tool.
     */
    FSingleCameraCutTrackEditor(TSharedRef<ISequencer> InSequencer);

    /** Virtual destructor. */
    virtual ~FSingleCameraCutTrackEditor() { }

    /**
     * Creates an instance of this class.  Called by a sequencer .
     *
     * @param OwningSequencer The sequencer instance to be used by this tool.
     * @return The new instance of this class.
     */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

public:

    // ISequencerTrackEditor interface

    virtual void BindCommands(TSharedRef<FUICommandList> SequencerCommandBindings) override;
    virtual void BuildAddTrackMenu(FMenuBuilder& MenuBuilder) override;
    virtual void BuildTrackContextMenu(FMenuBuilder& MenuBuilder, UMovieSceneTrack* Track) override;
    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface(UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding) override;
    virtual void OnRelease() override;
    virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
    virtual bool SupportsSequence(UMovieSceneSequence* InSequence) const override;
    virtual void Tick(float DeltaTime) override;
    virtual const FSlateBrush* GetIconBrush() const override;
    virtual bool OnAllowDrop(const FDragDropEvent& DragDropEvent, FSequencerDragDropParams& DragDropParams) override;
    virtual FReply OnDrop(const FDragDropEvent& DragDropEvent, const FSequencerDragDropParams& DragDropParams) override;

protected:

    /** Delegate for AnimatablePropertyChanged in AddKey */
    FKeyPropertyResult AddKeyInternal(FFrameNumber AutoKeyTime, const FGuid ObjectGuid);

    /** Find or create a camera cut track in the currently focused movie scene. */
    UMovieSceneSingleCameraCutTrack* FindOrCreateSingleCameraCutTrack();

    UFactory* GetAssetFactoryForNewSingleCameraCut(UClass* SequenceClass);

private:

    /** Callback for determining whether the "Add Camera Cut" menu entry can execute. */
    bool HandleAddSingleCameraCutTrackMenuEntryCanExecute() const;

    /** Callback for executing the "Add Camera Cut Track" menu entry. */
    void HandleAddSingleCameraCutTrackMenuEntryExecute();

    /** Callback for determining whether the "Add Camera Cut" menu entry is visible. */
    bool HandleAddSingleCameraCutTrackMenuEntryIsVisible();

    /** Callback for generating the menu of the "Add Camera Cut" combo button. */
    TSharedRef<SWidget> HandleAddSingleCameraCutComboButtonGetMenuContent();

    /** Callback for whether a camera is pickable. */
    bool IsCameraPickable( const AActor* const PickableActor );

    /** Callback for executing a menu entry in the "Add Camera Cut" combo button. */
    void HandleAddSingleCameraCutComboButtonMenuEntryExecute(AActor* Camera);

    /** Called to create a new section for the specified binding ID. */
    void CreateNewSectionFromBinding(FMovieSceneObjectBindingID InBindingID);

    /** Called when toggling whether the track can blend camera cuts. */
    //void HandleToggleCanBlendExecute(UMovieSceneSingleCameraCutTrack* CameraCutTrack);

    /** Delegate for camera button lock state */
    ECheckBoxState IsCameraLocked() const;

    /** Delegate for locked camera button */
    void OnLockCameraClicked(ECheckBoxState CheckBoxState);

    /** Toggle the state of the camera lock */
    void ToggleLockCamera();

    /** Delegate for camera button lock tooltip */
    FText GetLockCameraToolTip() const;

private:

    /** The Thumbnail pool which draws all the viewport thumbnails for the camera cut track. */
    TSharedPtr<FTrackEditorThumbnailPool> ThumbnailPool;

    /** A binding ID picker that allows us to create a new section from an existing binding */
    TSharedPtr<FTrackEditorBindingIDPicker> BindingIDPicker;
};
