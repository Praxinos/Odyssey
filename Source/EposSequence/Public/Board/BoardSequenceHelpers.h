// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

class ACineCameraActor;
class UMovieScene;
class UMovieScene3DTransformSection;
class UMovieScenePrimitiveMaterialSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class IMovieScenePlayer;
struct FMovieSceneChannelProxy;

class EPOSSEQUENCE_API BoardSequenceHelpers
{
public:
    struct FInnerSequenceResult
    {
        UMovieSceneSequence*    mInnerSequence = nullptr;
        FMovieSceneSequenceID   mInnerSequenceId = MovieSceneSequenceID::Invalid;
        UMovieScene*            mInnerMovieScene = nullptr;
        FFrameTime              mInnerTime = 0;
    };
    static FInnerSequenceResult GetInnerSequence( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceId );
    static FInnerSequenceResult GetInnerSequence( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber );

public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static TArray<FFrameTime> GetCameraTransformTimesRecursive( const UMovieSceneSubSection& iSubSection );

public:
    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );

    static TSharedPtr<FMovieSceneChannelProxy> BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
};
