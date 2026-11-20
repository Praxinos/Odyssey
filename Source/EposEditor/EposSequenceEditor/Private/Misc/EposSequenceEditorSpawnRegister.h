// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Misc/LevelSequenceEditorSpawnRegister.h"

#define ODYSSEY_API EPOSSEQUENCEEDITOR_API

class ACineCameraActor;
class FLevelEditorViewportClient;

/** Movie scene spawn register that knows how to handle spawning objects (actors) for a board/shot sequence  */
class FEposSequenceEditorSpawnRegister
    : public FLevelSequenceEditorSpawnRegister
{
public:
    ODYSSEY_API FEposSequenceEditorSpawnRegister();
    ODYSSEY_API ~FEposSequenceEditorSpawnRegister();

public:
    using FLevelSequenceEditorSpawnRegister::SpawnObject;
    ODYSSEY_API virtual UObject* SpawnObject( const FGuid& BindingId, UMovieScene& MovieScene, FMovieSceneSequenceIDRef Template, TSharedRef<const FSharedPlaybackState> SharedPlaybackState, int32 BindingIndex ) override;
    ODYSSEY_API virtual void PreDestroyObject( UObject& Object, const FGuid& BindingId, int32 BindingIndex, FMovieSceneSequenceIDRef TemplateID ) override;

protected:
    // Keep the public DestroySpawnedObject overloads of the base visible (the override below would hide them)
    using FLevelSequenceEditorSpawnRegister::DestroySpawnedObject;
    ODYSSEY_API virtual void DestroySpawnedObject( UObject& Object, UMovieSceneSpawnableBindingBase* CustomSpawnableBinding ) override;

private:
    void BindToLevelViewportClients();
    void HandleLevelViewportClientListChanged();
    void HandleLockChanged( FLevelEditorViewportClient* iViewportClient );
    void PilotCamera( FLevelEditorViewportClient& iViewportClient, ACineCameraActor& iCamera );

private:
    /** Lock-changed delegate handle of each level viewport client listened to */
    TMap<FLevelEditorViewportClient*, FDelegateHandle> LockChangedHandles;

    /** Viewports whose piloted camera was destroyed by this register: any camera spawned next is piloted there,
        until the user changes the lock of that viewport */
    TSet<FLevelEditorViewportClient*> ViewportsPilotingSpawnedCameras;

    /** True while this register changes viewport locks itself (piloting a spawned camera, or destroying a spawned object
        which makes SLevelViewport eject the pilot), so those lock changes are not taken as the user's */
    bool IgnoreLockChangedDelegate = false;

    FDelegateHandle ViewportClientListChangedHandle;
};

#undef ODYSSEY_API
