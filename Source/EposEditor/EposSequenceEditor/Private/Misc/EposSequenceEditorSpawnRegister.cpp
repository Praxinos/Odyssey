// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "EposSequenceEditorSpawnRegister.h"

#include "CineCameraActor.h"
#include "Editor.h"
#include "ISequencer.h"
#include "LevelEditorViewport.h"
#include "Misc/ScopeExit.h" // si tu préfères ON_SCOPE_EXIT à TGuardValue

FEposSequenceEditorSpawnRegister::FEposSequenceEditorSpawnRegister()
{
    if( GEditor )
    {
        ViewportClientListChangedHandle = GEditor->OnLevelViewportClientListChanged().AddRaw( this, &FEposSequenceEditorSpawnRegister::HandleLevelViewportClientListChanged );
        BindToLevelViewportClients();
    }
}

FEposSequenceEditorSpawnRegister::~FEposSequenceEditorSpawnRegister()
{
    if( GEditor )
    {
        GEditor->OnLevelViewportClientListChanged().Remove( ViewportClientListChangedHandle );

        // Only unbind from viewport clients that still exist
        for( FLevelEditorViewportClient* ViewportClient : GEditor->GetLevelViewportClients() )
        {
            if( const FDelegateHandle* handle = LockChangedHandles.Find( ViewportClient ) )
            {
                ViewportClient->OnLockChanged().Remove( *handle );
            }
        }
    }
}

//---

void
FEposSequenceEditorSpawnRegister::BindToLevelViewportClients()
{
    for( FLevelEditorViewportClient* ViewportClient : GEditor->GetLevelViewportClients() )
    {
        if( ViewportClient && !LockChangedHandles.Contains( ViewportClient ) )
        {
            LockChangedHandles.Add( ViewportClient, ViewportClient->OnLockChanged().AddRaw( this, &FEposSequenceEditorSpawnRegister::HandleLockChanged, client ) );
        }
    }
}
void
FEposSequenceEditorSpawnRegister::HandleLevelViewportClientListChanged()
{
    // Forget removed viewport clients without touching them: they are being (or have been) destroyed
    const TArray<FLevelEditorViewportClient*>& ViewportClients = GEditor->GetLevelViewportClients();
    for( auto it = LockChangedHandles.CreateIterator(); it; ++it )
    {
        if( !ViewportClients.Contains( it.Key() ) )
            it.RemoveCurrent();
    }
    for( auto it = ViewportsPilotingSpawnedCameras.CreateIterator(); it; ++it )
    {
        if( !ViewportClients.Contains( *it ) )
            it.RemoveCurrent();
    }

    BindToLevelViewportClients();
}

void
FEposSequenceEditorSpawnRegister::DestroySpawnedObject( UObject& Object, UMovieSceneSpawnableBindingBase* CustomSpawnableBinding )
{
    // Destroying the piloted camera makes SLevelViewport::OnLevelActorsRemoved() eject the pilot (SetActorLock(nullptr)):
    // that's not the user's choice, so keep piloting the next spawned camera
    TGuardValue<bool> guard( IgnoreLockChangedDelegate, true );

    FLevelSequenceEditorSpawnRegister::DestroySpawnedObject( Object, CustomSpawnableBinding );
}

void
FEposSequenceEditorSpawnRegister::HandleLockChanged( FLevelEditorViewportClient* iViewportClient )
{
    // The user piloted something else or ejected (or the camera cut took control back):
    // spawned cameras must no longer be piloted in this viewport (replaces the old ResetCameraLock())
    if( !IgnoreLockChangedDelegate )
    {
        ViewportsPilotingSpawnedCameras.Remove( iViewportClient );
    }
}

void
FEposSequenceEditorSpawnRegister::PilotCamera( FLevelEditorViewportClient& iViewportClient, ACineCameraActor& iCamera )
{
    // Same as MovieSceneToolHelpers::LockCameraActorToViewport(), but for the viewport that was piloting
    TGuardValue<bool> guard( IgnoreLockChangedDelegate, true );

    if( TSharedPtr<ISequencer> sequencer = GetSequencer() )
    {
        sequencer->SetPerspectiveViewportCameraCutEnabled( false );
    }

    iViewportClient.SetCinematicActorLock( nullptr );
    iViewportClient.SetActorLock( &iCamera );
    iViewportClient.bLockedCameraView = true;
    iViewportClient.UpdateViewForLockedActor();
    iViewportClient.Invalidate();
}

UObject*
FEposSequenceEditorSpawnRegister::SpawnObject( const FGuid& BindingId, UMovieScene& MovieScene, FMovieSceneSequenceIDRef TemplateID, TSharedRef<const FSharedPlaybackState> SharedPlaybackState, int32 BindingIndex/* = 0*/ )
{
    UObject* newObject = FLevelSequenceEditorSpawnRegister::SpawnObject( BindingId, MovieScene, TemplateID, SharedPlaybackState, BindingIndex );

    // If a camera was piloted in a viewport, pilot the new spawned camera there (whichever camera it is)
    ACineCameraActor* camera = Cast<ACineCameraActor>( newObject );
    if( camera && camera->GetLevel() && GEditor && ViewportsPilotingSpawnedCameras.Num() )
    {
        const TArray<FLevelEditorViewportClient*>& ViewportClients = GEditor->GetLevelViewportClients();
        for( FLevelEditorViewportClient* ViewportClient : ViewportsPilotingSpawnedCameras )
        {
            if( ViewportClients.Contains( ViewportClient ) )
            {
                PilotCamera( *ViewportClient, *camera );
            }
        }
    }

    return newObject;
}

void
FEposSequenceEditorSpawnRegister::PreDestroyObject( UObject& Object, const FGuid& BindingId, int32 BindingIndex, FMovieSceneSequenceIDRef TemplateID )
{
    // Check only destroyed camera actor (to not loop over all other actors in the sequence)
    if( Cast<ACineCameraActor>( &Object ) && GEditor )
    {
        // If the piloted camera is the one being destroyed, mark the viewport as "was piloting"
        // (the user lock, not the cinematic lock of the camera cut)
        for( FLevelEditorViewportClient* ViewportClient : GEditor->GetLevelViewportClients() )
        {
            if( ViewportClient && ViewportClient->GetActiveActorLock().Get() == &Object )
            {
                ViewportsPilotingSpawnedCameras.Add( ViewportClient );
            }
        }
    }

    FLevelSequenceEditorSpawnRegister::PreDestroyObject( Object, BindingId, BindingIndex, TemplateID );
}
