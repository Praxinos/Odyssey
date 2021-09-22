// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameNumber.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

class ACineCameraActor;
class APlaneActor;
class UMaterialInstanceConstant;
class UMovieScene;
class UMovieScene3DTransformSection;
class UMovieScenePrimitiveMaterialSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UWorld;
class IMovieScenePlayer;
struct FMovieSceneChannelProxy;
struct FMovieSceneObjectPathChannel;

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

enum class EGetPlane
{
    kAll,
    kSelectedOnly,
    kSelectedOrAll,
};

class EPOSSEQUENCE_API ShotSequenceHelpers
{
public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static int32 GetAllPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    static int32 GetAttachedPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    struct FDrawingData
    {
        FMovieSceneObjectPathChannel* mChannel;
        UMovieSceneSection* mSection;
        int32 mKeyIndex;
    };
    static int32 GetDrawingIndex( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding, FDrawingData* oData = nullptr );
    static int32 GetAllDrawings( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TArray<FDrawingData>* oDrawings );
    static TArray<FFrameNumber> GetAllDrawingTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection );

    static TArray<FFrameNumber> GetCameraTransformTimes( UMovieSceneSequence* iSequence );

public:
    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );

    static TSharedPtr<FMovieSceneChannelProxy> BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
};
