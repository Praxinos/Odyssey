// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Sections/ThumbnailSection.h"
#include "TrackEditors/SubTrackEditorBase.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

//---


struct FMetaFloatChannel;
struct FMetaMaterialChannel;

class CinematicBoardSectionKeysHelpers
{
public:
    static TSharedPtr<FMovieSceneChannelProxy> BuildCameraTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TSharedPtr<FMetaFloatChannel> BuildCameraTransformMetaChannel( const TSharedPtr<FMovieSceneChannelProxy> iChannelProxy, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesTransformChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMetaFloatChannel>> BuildPlanesTransformMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

    static TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> BuildPlanesMaterialChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, TSharedPtr<FMetaMaterialChannel>> BuildPlanesMaterialMetaChannel( const TMap<FGuid, TSharedPtr<FMovieSceneChannelProxy>> iChannelProxies, const FFrameNumber& iMergeTolerance );

private:
};
