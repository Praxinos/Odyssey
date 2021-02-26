// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutHelpers.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTrack.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieScenePrimitiveMaterialTrack.h"

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

//static
TArray<FFrameTime>
MovieSceneSingleCameraCutHelpers::GetCameraTransformKeys( UMovieSceneSequence* iShotSequence )
{
    TArray<FFrameTime> keys;

    UMovieScene* movie_scene = iShotSequence->GetMovieScene();
    if( !movie_scene )
        return keys;

    UMovieSceneTrack* cameracut_track = movie_scene->GetCameraCutTrack();
    if( !cameracut_track )
        return keys;

    TArray<UMovieSceneSection*> cameracut_sections = cameracut_track->GetAllSections();
    if( !cameracut_sections.Num() )
        return keys;

    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( cameracut_sections[0] );
    if( !cameracut_section )
        return keys;

    UMovieSceneTrack* track = movie_scene->FindTrack<UMovieScene3DTransformTrack>( cameracut_section->GetCameraBindingID().GetGuid() );
    if( !track )
        return keys;

    for( auto section : track->GetAllSections() )
    {
        TArrayView<FMovieSceneFloatChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
        for( int i = 0; i < 9; i++ )
        {
            TArrayView<const FFrameNumber> times = channels[i]->GetTimes();
            for( auto time : times )
            {
                TRange<FFrameNumber> range( cameracut_section->GetTrueRange() );
                range.SetUpperBound( TRangeBound<FFrameNumber>::FlipInclusion( range.GetUpperBound() ) ); // Special case when a key is on the frame just on the exclusive upper bound value to render it
                if( range.Contains( time ) )
                    keys.AddUnique( time );
            }
        }
    }

    return keys;
}

//static
TArray<FFrameTime>
MovieSceneSingleCameraCutHelpers::GetPlaneTransformKeys( UMovieSceneSequence* iShotSequence, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieScene* movie_scene = iShotSequence->GetMovieScene();
    if( !movie_scene )
        return keys;

    UMovieSceneTrack* cameracut_track = movie_scene->GetCameraCutTrack();
    if( !cameracut_track )
        return keys;

    TArray<UMovieSceneSection*> cameracut_sections = cameracut_track->GetAllSections();
    if( !cameracut_sections.Num() )
        return keys;

    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( cameracut_sections[0] ); // It's only to get keys inside its range
    if( !cameracut_section )
        return keys;

    //---

    UMovieSceneTrack* track = movie_scene->FindTrack<UMovieScene3DTransformTrack>( iPossessable.GetGuid() );
    if( !track )
        return keys;

    for( auto section : track->GetAllSections() )
    {
        TArrayView<FMovieSceneFloatChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
        for( int i = 0; i < 9; i++ )
        {
            TArrayView<const FFrameNumber> times = channels[i]->GetTimes();
            for( auto time : times )
            {
                TRange<FFrameNumber> range( cameracut_section->GetTrueRange() );
                range.SetUpperBound( TRangeBound<FFrameNumber>::FlipInclusion( range.GetUpperBound() ) ); // Special case when a key is on the frame just on the exclusive upper bound value to render it
                if( range.Contains( time ) )
                    keys.AddUnique( time );
            }
        }
    }

    return keys;
}

//static
TArray<FFrameTime>
MovieSceneSingleCameraCutHelpers::GetPlaneMaterialKeys( UMovieSceneSequence* iShotSequence, FMovieScenePossessable iPossessable )
{
    TArray<FFrameTime> keys;

    UMovieScene* movie_scene = iShotSequence->GetMovieScene();
    if( !movie_scene )
        return keys;

    UMovieSceneTrack* cameracut_track = movie_scene->GetCameraCutTrack();
    if( !cameracut_track )
        return keys;

    TArray<UMovieSceneSection*> cameracut_sections = cameracut_track->GetAllSections();
    if( !cameracut_sections.Num() )
        return keys;

    UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( cameracut_sections[0] ); // It's only to get keys inside its range
    if( !cameracut_section )
        return keys;

    //---

    FMovieScenePossessable possessable;
    for( int i = 0; i < movie_scene->GetPossessableCount(); i++ )
    {
        FMovieScenePossessable p = movie_scene->GetPossessable( i );
        if( p.GetParent() == iPossessable.GetGuid() )
        {
            possessable = p;
            break;
        }
    }

    UMovieSceneTrack* track = movie_scene->FindTrack<UMovieScenePrimitiveMaterialTrack>( possessable.GetGuid() );
    if( !track )
        return keys;

    for( auto section : track->GetAllSections() )
    {
        TArrayView<FMovieSceneObjectPathChannel*> channels = section->GetChannelProxy().GetChannels<FMovieSceneObjectPathChannel>();

        TArrayView<const FFrameNumber> times = channels[0]->GetData().GetTimes();
        for( auto time : times )
        {
            TRange<FFrameNumber> range( cameracut_section->GetTrueRange() );
            range.SetUpperBound( TRangeBound<FFrameNumber>::FlipInclusion( range.GetUpperBound() ) ); // Special case when a key is on the frame just on the exclusive upper bound value to render it
            if( range.Contains( time ) )
                keys.AddUnique( time );
        }
    }

    return keys;
}
