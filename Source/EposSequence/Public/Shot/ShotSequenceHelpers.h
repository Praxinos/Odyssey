// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameNumber.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

class ACineCameraActor;
class AStaticMeshActor;
class UMaterialInstanceConstant;
class UMovieScene;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UWorld;
class IMovieScenePlayer;
struct FMovieSceneObjectPathChannel;

class EPOSSEQUENCE_API ShotSequenceHelpers
{
public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static int32 GetPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    struct FDrawingData
    {
        FMovieSceneObjectPathChannel* mChannel;
        UMovieSceneTrack* mTrack;
        UMovieSceneSection* mSection;
    };
    static int32 GetDrawingIndex( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding, FDrawingData* oData = nullptr );
    static TArray<FFrameNumber> GetAllMaterialTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static TArray<FFrameTime> GetCameraTransformKeys( UMovieSceneSequence* iSequence );
    static TArray<FFrameTime> GetCameraTransformKeysRecursive( const UMovieSceneSubSection& iSubSection );
};
