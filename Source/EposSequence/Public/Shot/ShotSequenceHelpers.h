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
class UMovieScene3DTransformSection;
class UMovieScenePrimitiveMaterialSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UWorld;
class IMovieScenePlayer;
struct FMovieSceneObjectPathChannel;

enum class EGetPlane
{
    kAlwaysAll,
    kSelectedOnly,
    kSelectedOrAll,
};

class EPOSSEQUENCE_API ShotSequenceHelpers
{
public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static int32 GetPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iSelection, TArray<AStaticMeshActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    struct FDrawingData
    {
        FMovieSceneObjectPathChannel* mChannel;
        UMovieSceneTrack* mTrack;
        UMovieSceneSection* mSection;
    };
    static int32 GetDrawingIndex( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding, FDrawingData* oData = nullptr );
    static TArray<FFrameNumber> GetAllMaterialTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static TArray<FFrameNumber> GetCameraTransformTimes( UMovieSceneSequence* iSequence );

public:
    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );
};
