// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/FrameNumber.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

#include "EposSequenceHelpers.generated.h"

class ACineCameraActor;
class APlaneActor;
class UMaterialInstance;
class UMaterialInstanceConstant;
class UMovieScene;
class UMovieScene3DTransformSection;
class UMovieSceneBoolSection;
class UMovieSceneComponentMaterialTrack;
class UMovieSceneNoteSection;
class UMovieSceneParameterSection;
class UMovieScenePrimitiveMaterialSection;
class UMovieScenePrimitiveMaterialTrack;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class UMovieSceneVisibilityTrack;
class UStoryNote;
class UWorld;
class IMovieScenePlayer;
struct FMovieSceneChannelHandle;
struct FMovieSceneChannelProxy;
struct FMovieSceneFloatChannel;
struct FMovieSceneObjectPathChannel;


class UMoviePipeline;

UCLASS()
class EPOSSEQUENCE_API UMoviePipelineStoryboardBlueprintLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Get all notes at the current frame. */
    UFUNCTION( BlueprintPure, Category = "Movie Render Pipeline Storyboard" )
    static TArray<UStoryNote*> GetNotes( const UMoviePipeline* MoviePipeline );
};



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
    static FChannelProxyBySectionMap BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesOpacityChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
};

enum class EGetPlane
{
    kAll,
    kSelectedOnly,
    kSelectedOrAll,
};

struct EPOSSEQUENCE_API FDrawing
{
    FMovieSceneObjectPathChannel*       mChannel { nullptr };
    TWeakObjectPtr<UMovieSceneSection>  mSection;
    FKeyHandle                          mKeyHandle { FKeyHandle::Invalid() };

    bool Exists();

    UMaterialInstance* GetMaterial() const;

    void SetMaterial( UMaterialInstance* iMaterial );

    friend EPOSSEQUENCE_API bool operator==( const FDrawing& iLhs, const FDrawing& iRhs );
};

struct EPOSSEQUENCE_API FKeyOpacity
{
    FMovieSceneFloatChannel*            mChannel { nullptr };
    TWeakObjectPtr<UMovieSceneSection>  mSection;
    FKeyHandle                          mKeyHandle { FKeyHandle::Invalid() };

    bool Exists();

    bool GetOpacity( float& oOpacity );

    void SetOpacity( float iOpacity );
};

class EPOSSEQUENCE_API ShotSequenceHelpers
{
public:
    static ACineCameraActor* GetCamera( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid* oCameraBinding = nullptr );

    static TArray<FFrameNumber> GetCameraTransformTimes( UMovieSceneSequence* iSequence );

    static int32 GetAllPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );
    static int32 GetAttachedPlanes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection, TArray<APlaneActor*>* oPlanes, TArray<FGuid>* oPlaneBindings );

public:
    struct FFindOrCreatePlaneVisibilityResult
    {
        TWeakObjectPtr<UMovieSceneVisibilityTrack>      mTrack;
        bool mTrackCreated { false };
        TArray<TWeakObjectPtr<UMovieSceneBoolSection>>  mSections;
        bool mSectionsCreated { false };
    };
    static FFindOrCreatePlaneVisibilityResult           FindPlaneVisibilityTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );
    //static FFindOrCreatePlaneVisibilityResult         FindOrCreatePlaneVisibilityTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );

public:
    struct FFindOrCreateMaterialDrawingResult
    {
        TWeakObjectPtr<UMovieScenePrimitiveMaterialTrack>           mTrack;
        bool mTrackCreated { false };
        TArray<TWeakObjectPtr<UMovieScenePrimitiveMaterialSection>> mSections;
        bool mSectionsCreated { false };

        FGuid mPlaneComponentBinding; // The binding of the root component of the plane
    };
    static FFindOrCreateMaterialDrawingResult         FindMaterialDrawingTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );
    static FFindOrCreateMaterialDrawingResult         FindOrCreateMaterialDrawingTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );

    static FDrawing             GetDrawing( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );
    static TArray<FDrawing>     GetAllDrawings( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding );
    static TArray<FFrameNumber> GetAllDrawingTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGetPlane iPlaneSelection );

    static FDrawing             ConvertToDrawing( TWeakObjectPtr<UMovieSceneSection> iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle );

public:
    struct FFindOrCreateMaterialParameterResult
    {
        TWeakObjectPtr<UMovieSceneComponentMaterialTrack>   mTrack;
        bool mTrackCreated { false };
        TArray<TWeakObjectPtr<UMovieSceneParameterSection>> mSections;
        bool mSectionsCreated { false };

        FGuid mPlaneComponentBinding; // The binding of the root component of the plane
    };
    static FFindOrCreateMaterialParameterResult         FindMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );
    static FFindOrCreateMaterialParameterResult         FindOrCreateMaterialParameterTrackAndSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TOptional<FFrameNumber> iFrameNumber = TOptional<FFrameNumber>() );

    struct FFindOrCreateParameterChannelResult
    {
        // In the future, instead of maybe using a generic FMovieSceneChannel*,
        // we can use multiple data (one for FMovieSceneFloatChannel* and one for FMovieSceneVectorChannel*)
        // because in the material track gui, we can only use material parameter which are float and color
        // so it should be ok to have 2 data members instead of a generic single one which will be recast when needed
        //
        // or maybe use (also 2 members) FScalarParameterNameAndCurve and FColorParameterNameAndCurve instead of inner channel ?
        FMovieSceneFloatChannel* mChannel { nullptr };
        bool mChannelCreated { false };
    };
    static FFindOrCreateParameterChannelResult          FindMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TWeakObjectPtr<UMovieSceneParameterSection> iSection );
    static FFindOrCreateParameterChannelResult          FindOrCreateMaterialOpacityChannel( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iPlaneBinding, TWeakObjectPtr<UMovieSceneParameterSection> iSection );

    static FKeyOpacity                                  GetOpacityKey( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FFrameNumber iFrameNumber, FGuid iPlaneBinding );

    static FKeyOpacity                                  ConvertToOpacityKey( TWeakObjectPtr<UMovieSceneSection> iSection, const FMovieSceneChannelHandle& iChannelHandle, FKeyHandle iKeyHandle );

public:
    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iCameraBinding );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FGuid& iPlaneBinding );

    static FChannelProxyBySectionMap BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap> BuildPlanesOpacityChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
};
