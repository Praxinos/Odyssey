// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---

class UMovieScene3DTransformSection;
class UMovieScenePrimitiveMaterialSection;

class CinematicBoardSectionBindingHelpers
{
public:
    /** Get all (static mesh) possessables inside the given section
        This is used by GetMaxPlaneBindings() which loops over all sections in the track
        to get section with the max number of planes
    */
    static TArray<FMovieScenePossessable> GetPlaneBindings( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID );

    /** Get the maximum number of planes inside all subsections of the current track */
    static int GetMaxPlaneBindings( IMovieScenePlayer& iPlayer, const UMovieSceneTrack& iTrack, FMovieSceneSequenceIDRef iSequenceID );

    /** Get the camera possessable inside the current subsection */
    static FMovieScenePossessable GetCameraBinding( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID );
};

//---

struct FMetaFloatChannel;
struct FMetaMaterialChannel;

class CinematicBoardSectionKeysHelpers
{
public:
    static TArray<double> BuildThumbnailKeys( const UMovieSceneSubSection& iSubSection );

    static TSharedPtr<FMovieSceneChannelProxy> BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TSharedPtr<FMetaFloatChannel> BuildCameraTransformMetaChannel( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMetaFloatChannel>> BuildPlanesTransformMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMetaMaterialChannel>> BuildPlanesMaterialMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

private:
    static TArray<FFrameTime> FindCameraTransformKeysRecursive( const UMovieSceneSubSection& iSubSection );

    static TArray<UMovieScene3DTransformSection*> GetCameraTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable );
    static TArray<UMovieScene3DTransformSection*> GetPlaneTransformSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable );
    static TArray<UMovieScenePrimitiveMaterialSection*> GetPlaneMaterialSections( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSection, FMovieSceneSequenceIDRef iSequenceID, const FMovieScenePossessable& iPossessable );
};
