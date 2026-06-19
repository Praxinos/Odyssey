// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Misc/LevelSequenceEditorSpawnRegister.h"

#define ODYSSEY_API EPOSSEQUENCEEDITOR_API

/** Movie scene spawn register that knows how to handle spawning objects (actors) for a board/shot sequence  */
class FEposSequenceEditorSpawnRegister
    : public FLevelSequenceEditorSpawnRegister
{
public:
    //PATCH
    // Because WeakSequencer is private -_-
    ODYSSEY_API void SetSequencer( const TSharedPtr<ISequencer>& Sequencer );

public:
    ODYSSEY_API virtual UObject* SpawnObject( const FGuid& BindingId, UMovieScene& MovieScene, FMovieSceneSequenceIDRef Template, TSharedRef<const FSharedPlaybackState> SharedPlaybackState, int32 BindingIndex ) override;
    ODYSSEY_API virtual void PreDestroyObject( UObject& Object, const FGuid& BindingId, int32 BindingIndex, FMovieSceneSequenceIDRef TemplateID ) override;

    ODYSSEY_API virtual void ResetCameraLock();

private:
    //PATCH
    // Because WeakSequencer is private -_-
    TWeakPtr<ISequencer> WeakSequencer2;

    TOptional<FMovieSceneSpawnRegisterKey> ActiveCameraLocked;
};
