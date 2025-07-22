// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "KeyParams.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"
#include "Tracks/MovieSceneMaterialTrack.h"
#include "TransformData.h"

class AActor;
class ACineCameraActor;
class UMaterialInstanceConstant;
class UMovieScene;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneTrack;
class IMovieScenePlayer;
class ISequencer;

class ToolkitHelpers
{
public:
    /**
    *  Create all default inner sub tracks of added actor
    *
    * @param ISequencer iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void CreateDefaultTracksForActor( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

    /**
    *  Fix the binding of the cameracut track when a new camera is dropped
    *
    * @param ISequencer iSequencer to add inner track.
    * @param AActor iActor corresponding to the binding.
    * @param FGuid iBinding  Guid of the track containing the actor.
    */
    static void FixCameraBindingOnCameraCut( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

    // PATCH
    static void PatchStandardCameraCutTrack( ISequencer* iSequencer, AActor* iActor, const FGuid iBinding );

private:
    static UMovieSceneTrack* CreateTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, FComponentMaterialInfo iMaterialTrackInfo = FComponentMaterialInfo() );
    //static FGuid CreateComponentTrack( ISequencer* iSequencer, AActor* iActor, const FString& iComponentName );
    static FGuid CreateComponentTrack( ISequencer* iSequencer, AActor* iActor, UActorComponent* iComponent );
    static void CreatePropertyTrack( ISequencer* iSequencer, AActor* iActor, const FGuid& iBinding, UClass* iClass, const FString& iComponentPath, const FString& iPropertyPath );

public:
    static void SetStoryboardViewport();

public:
    static void HandleActorAddedToSequencer( AActor* iActor, const FGuid iBinding, ISequencer* iSequencer );
    static void HandleOnActivateSequence( FMovieSceneSequenceIDRef iSequenceID, ISequencer* iSequencer );
    static void HandleOnSelectionChangedSections( TArray<UMovieSceneSection*> iSections, ISequencer* iSequencer );
};
