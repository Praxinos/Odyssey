// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/FrameNumber.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

class ACineCameraActor;
class APlaneActor;
class UMaterialInstance;
class UMaterialInstanceConstant;
class UMovieScene;
class UMovieScene3DTransformSection;
class UMovieSceneNoteSection;
class UMovieScenePrimitiveMaterialSection;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UStoryNote;
class UWorld;
class IMovieScenePlayer;
struct FMovieSceneChannelProxy;
struct FMovieSceneObjectPathChannel;

typedef TMap<TWeakObjectPtr<UMovieSceneSection>, TSharedPtr<FMovieSceneChannelProxy>> FChannelProxyBySectionMap;

class EPOSSEQUENCE_API EposSequenceHelpers
{
public:
    static TArray<TWeakObjectPtr<UMovieSceneNoteSection>> GetNotesRecursive( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber );

    static TArray<TWeakObjectPtr<UMovieSceneNoteSection>> GetNotes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );
};

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

    static FChannelProxyBySectionMap BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
};

enum class EGetPlane
{
    kAll,
    kSelectedOnly,
    kSelectedOrAll,
};

struct EPOSSEQUENCE_API FDrawing
{
    FMovieSceneObjectPathChannel*   mChannel { nullptr };
    UMovieSceneSection*             mSection { nullptr };
    int32                           mKeyIndex { INDEX_NONE };
    FKeyHandle                      mKeyHandle;

    bool Exists();

    UMaterialInstance* GetMaterial();

    void SetMaterial( UMaterialInstance* iMaterial );
};

class EPOSSEQUENCE_API ShotSequenceHelpers
{
public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static int32 GetAllPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    static int32 GetAttachedPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );

    static FDrawing             GetDrawing( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static TArray<FDrawing>     GetAllDrawings( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static TArray<FFrameNumber> GetAllDrawingTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection );

    static TArray<FFrameNumber> GetCameraTransformTimes( UMovieSceneSequence* iSequence );

public:
    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );

    static FChannelProxyBySectionMap BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
};
