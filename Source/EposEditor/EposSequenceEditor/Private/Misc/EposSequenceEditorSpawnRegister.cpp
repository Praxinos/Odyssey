// IDDN.FR.000.000000.000.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "EposSequenceEditorSpawnRegister.h"

#include "CineCameraActor.h"
#include "LevelEditorViewport.h"
#include "MovieSceneToolHelpers.h"

void
FEposSequenceEditorSpawnRegister::SetSequencer( const TSharedPtr<ISequencer>& Sequencer )
{
    FLevelSequenceEditorSpawnRegister::SetSequencer( Sequencer );
    WeakSequencer2 = Sequencer;
}

UObject*
FEposSequenceEditorSpawnRegister::SpawnObject( const FGuid& BindingId, UMovieScene& MovieScene, FMovieSceneSequenceIDRef TemplateID, TSharedRef<const FSharedPlaybackState> SharedPlaybackState, int32 BindingIndex/* = 0*/ )
{
    UObject* newObject = FLevelSequenceEditorSpawnRegister::SpawnObject( BindingId, MovieScene, TemplateID, SharedPlaybackState, BindingIndex );

    if( ACineCameraActor* camera = Cast<ACineCameraActor>( newObject ) )
    {
        // If a camera "track" was piloted, then pilot the new spawned camera
        if( ActiveCameraLocked.IsSet() )
        {
            MovieSceneToolHelpers::LockCameraActorToViewport( WeakSequencer2.Pin(), camera );

            FMovieSceneSpawnRegisterKey key( TemplateID, BindingId, BindingIndex );
            ActiveCameraLocked = key;
        }
    }

    return newObject;
}

void
FEposSequenceEditorSpawnRegister::PreDestroyObject( UObject& Object, const FGuid& BindingId, int32 BindingIndex, FMovieSceneSequenceIDRef TemplateID )
{
    // Check only destroyed camera actor (to not loop over all other actors in the sequence)
    if( ACineCameraActor* camera = Cast<ACineCameraActor>( &Object ) )
    {
        // Check if a camera is currently being piloted
        if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->IsAnyActorLocked() )
        {
            // Check if the piloted camera is object (actor) which will be destroyed, then mark the camera "track" as "was piloted"
            AActor* current_piloted_actor = GCurrentLevelEditingViewportClient->GetActiveActorLock().Get();
            if( current_piloted_actor == &Object )
            {
                FMovieSceneSpawnRegisterKey key( TemplateID, BindingId, BindingIndex );
                ActiveCameraLocked = key;
            }
        }
    }

    FLevelSequenceEditorSpawnRegister::PreDestroyObject( Object, BindingId, BindingIndex, TemplateID );
}

void
FEposSequenceEditorSpawnRegister::ResetCameraLock()
{
    //TODO: should rely on a delegate when a camera starts to be piloted
    // (like done in FLevelSequenceEditorSpawnRegister with selected actors)
    ActiveCameraLocked.Reset();
}
